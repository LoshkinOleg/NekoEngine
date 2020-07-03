#include "minelib/chunks/chunk_renderer.h"

#include <graphics/camera.h>
#ifdef EASY_PROFILE_USE
#include <easy/profiler.h>
#endif

#include "gl/texture.h"
#include "minelib/minecraft_like_engine.h"

namespace neko
{
ChunkRenderer::ChunkRenderer(
	MinecraftLikeEngine& engine,
	MoveableCamera3D& camera,
	EntityViewer& entityViewer)
	: camera_(camera),
	engine_(engine),
	entityViewer_(entityViewer)
{
}

void ChunkRenderer::Init()
{
	const auto& config = BasicEngine::GetInstance()->config;
	shader_.LoadFromFile(
		config.dataRootPath + "shaders/minecraft_like/light.vert",
		config.dataRootPath + "shaders/minecraft_like/light.frag");
	texture_[0] = gl::stbCreateTexture(config.dataRootPath + "sprites/blocks/dirt.jpg");
	texture_[1] = gl::stbCreateTexture(config.dataRootPath + "sprites/blocks/stone.jpg");
	texture_[2] = gl::stbCreateTexture(config.dataRootPath + "sprites/blocks/diamond_ore.jpg");
	cube_.Init();

	glEnable(GL_DEPTH_TEST);
}

void ChunkRenderer::DrawImGui(){

	ImGui::Begin("Light");
		Vec3f lightDirection = directionalLight_.direction_;
		if(ImGui::InputFloat3("Direction", &lightDirection[0])) {
			directionalLight_.direction_ = lightDirection;
		}

		Vec3f lightPosition = directionalLight_.position_;
		if (ImGui::InputFloat3("Position", &lightPosition[0])) {
			directionalLight_.position_ = lightPosition;
		}
	
	ImGui::End();
}


void ChunkRenderer::Update(seconds dt)
{
	std::lock_guard<std::mutex> lock(updateMutex_);
	GizmosLocator::get().DrawCube(directionalLight_.position_, Vec3f(.5f), Color4(1, 1, 1, 1));
}

void ChunkRenderer::Render()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("ChunkRenderer::Render");
#endif
	if (shader_.GetProgram() == 0) return;

	std::lock_guard<std::mutex> lock(updateMutex_);
	
	Mat4f view = camera_.GenerateViewMatrix();
	Mat4f projection = camera_.GenerateProjectionMatrix();
	shader_.Bind();
	SetLightParameters();

	for (size_t i = 0; i < INIT_ENTITY_NMB; i++)
	{
		if (!engine_.entityManager_.HasComponent(i, static_cast<EntityMask>(ComponentType::CHUNK))) { continue; }
		Chunk chunk = engine_.componentsManagerSystem_.chunkManager_.GetComponent(i);

#ifdef EASY_PROFILE_USE
		EASY_BLOCK("ChunkRenderer::Render::Chunk");
#endif

		for (int x = 0; x < kChunkSize; x++)
		{
			for (int y = 0; y < kChunkSize; y++)
			{
				for (int z = 0; z < kChunkSize; z++)
				{
#ifdef EASY_PROFILE_USE
					EASY_BLOCK("ChunkRenderer::Render::Air");
#endif
					int blockID = chunk.GetBlockId(Vec3i(x, y, z));
					if (blockID != 0)
					{
						if (texture_[blockID - 1] == INVALID_TEXTURE_ID) continue;
#ifdef EASY_PROFILE_USE
						EASY_BLOCK("ChunkRenderer::Render::Cube");
#endif
						Mat4f model = Mat4f::Identity;	
						model = Transform3d::Translate(model, Vec3f(x, y, z) + chunk.GetChunkPos());
				
						SetCameraParameters(model, view, projection, camera_.position);
						
						glBindTexture(GL_TEXTURE_2D, texture_[blockID - 1]); //bind texture id to texture slot
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
						
						cube_.Draw();
					}
				}
			}
		}
	}
}

void ChunkRenderer::SetCameraParameters(Mat4f& model, Mat4f& view, Mat4f& projection, Vec3f pos) {
	shader_.SetMat4("model", model);
	shader_.SetMat4("view", view);
	shader_.SetMat4("projection", projection);
	shader_.SetVec3("viewPos", pos);
	
	const auto inverseTransposeModel = model.Inverse().Transpose();
	shader_.SetMat4("inverseTransposeModel", inverseTransposeModel);
}

	
void ChunkRenderer::SetLightParameters() {
	shader_.SetVec3("light.color", directionalLight_.color_);
	shader_.SetVec3("light.direction", directionalLight_.direction_.Normalized());
	shader_.SetInt("objectMaterial.diffuse", 0);
	shader_.SetInt("objectMaterial.specular", 1);
	shader_.SetInt("objectMaterial.shininess", directionalLight_.specularPow_);

	shader_.SetFloat("ambientStrength", directionalLight_.ambientStrength_);
	shader_.SetFloat("diffuseStrength", directionalLight_.diffuseStrength_);
	shader_.SetFloat("specularStrength", directionalLight_.specularStrength_);
}
	
void ChunkRenderer::Destroy()
{
	cube_.Destroy();
	shader_.Destroy();
	gl::DestroyTexture(texture_[0]);
	gl::DestroyTexture(texture_[1]);
	gl::DestroyTexture(texture_[2]);
}

}