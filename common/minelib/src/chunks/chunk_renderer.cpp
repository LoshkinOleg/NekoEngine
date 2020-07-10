#include "minelib/chunks/chunk_renderer.h"

#include <graphics/camera.h>
#ifdef EASY_PROFILE_USE
#include <easy/profiler.h>
#endif

#include "gl/texture.h"
#include "minelib/minecraft_like_engine.h"

namespace neko
{
ChunkRenderer::ChunkRenderer(MinecraftLikeEngine& engine, Camera& camera)
	: camera_(camera),
	  engine_(engine),
	  chunkStatusManager_(engine.componentsManagerSystem.chunkStatusManager),
	  chunkContentManager_(engine.componentsManagerSystem.chunkContentManager),
	  chunkRenderManager_(engine.componentsManagerSystem.chunkRenderManager),
	  chunkPosManager_(engine.componentsManagerSystem.chunkPosManager)
{
}

void ChunkRenderer::Init()
{
	const auto& config = BasicEngine::GetInstance()->config;
	/*shader_.LoadFromFile(
		config.dataRootPath + "shaders/minecraft_like/light.vert",
		config.dataRootPath + "shaders/minecraft_like/light.frag");*/
	shader_.LoadFromFile(
		config.dataRootPath + "shaders/minecraft_like/base/cube_vertex.vert",
		config.dataRootPath + "shaders/minecraft_like/base/cube.frag");
	atlasTex_ = stbCreateTexture(config.dataRootPath + "sprites/atlas.png", gl::Texture::CLAMP_WRAP);
}

void ChunkRenderer::DrawImGui()
{
	ImGui::Begin("Light");
	Vec3f lightDirection = directionalLight_.direction_;
	if (ImGui::InputFloat3("Direction", &lightDirection[0]))
	{
		directionalLight_.direction_ = lightDirection;
	}

	Vec3f lightPosition = directionalLight_.position_;
	if (ImGui::InputFloat3("Position", &lightPosition[0]))
	{
		directionalLight_.position_ = lightPosition;
	}

	ImGui::End();
}

void ChunkRenderer::Update(seconds dt)
{
	GizmosLocator::get().DrawCube(directionalLight_.position_, Vec3f(.5f), Color4(1, 1, 1, 1));
}

void ChunkRenderer::Render()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("ChunkRenderer::Render");
#endif
	/*if (shader_.GetProgram() == 0) return;

	Mat4f view = camera_.GenerateViewMatrix();
	Mat4f projection = camera_.GenerateProjectionMatrix();
	shader_.Bind();
	SetLightParameters();

	const auto visibleChunks = chunkStatusManager_.GetVisibleChunks();
	for (auto visibleChunk : visibleChunks)
	{
		const Vec3f chunkPos = transform3dManager_.GetPosition(visibleChunk);
		for (int x = 0; x < kChunkSize; x++)
		{
			for (int y = 0; y < kChunkSize; y++)
			{
				for (int z = 0; z < kChunkSize; z++)
				{
					const int blockId = chunkContentManager_.GetBlockId(visibleChunk, Vec3i(x, y, z));
					if (blockId != 0)
					{
						if (texture_[blockId - 1] == INVALID_TEXTURE_ID) continue;
						Mat4f model = Mat4f::Identity;
						model = Transform3d::Translate(model, Vec3f(x, y, z) + chunkPos);

						SetCameraParameters(model, view, projection, camera_.position);

						glBindTexture(GL_TEXTURE_2D, texture_[blockId - 1]);
						//bind texture id to texture slot
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

						cube_.Draw();
					}
				}
			}
		}
	}*/
	
	shader_.Bind();
	SetCameraParameters(camera_);
	glBindTexture(GL_TEXTURE_2D, atlasTex_);
	const auto visibleChunks = chunkStatusManager_.GetVisibleChunks();
	for (auto& chunk : visibleChunks)
	{
		shader_.SetVec3("chunkPos", Vec3f(chunkPosManager_.GetComponent(chunk)));
		chunkRenderManager_.Draw(chunk);
	}
}

void ChunkRenderer::SetCameraParameters(const Camera& camera) const
{
	shader_.SetMat4("view", camera.GenerateViewMatrix());
	shader_.SetMat4("projection", camera.GenerateProjectionMatrix());
	shader_.SetVec3("viewPos", camera.position);
}

void ChunkRenderer::SetLightParameters() const
{
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
	shader_.Destroy();
	gl::DestroyTexture(atlasTex_);
}
}
