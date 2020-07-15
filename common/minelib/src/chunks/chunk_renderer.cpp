#include "minelib/chunks/chunk_renderer.h"

#ifdef EASY_PROFILE_USE
#include <easy/profiler.h>
#endif

#include <stb_image.h>

#include "gl/texture.h"
#include "graphics/camera.h"
#include "minelib/minecraft_like_engine.h"

namespace neko
{
ChunkRenderer::ChunkRenderer(MinecraftLikeEngine& engine, Camera& camera)
	: camera_(camera),
	  engine_(engine),
	  chunkManager_(engine.componentsManagerSystem.chunkManager)
{
}

void ChunkRenderer::Init()
{
	stbi_set_flip_vertically_on_load(true);
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
	//GizmosLocator::get().DrawCube(directionalLight_.position_, Vec3f(.5f), Color4(1, 1, 1, 1));
}

void ChunkRenderer::Render()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("ChunkRenderer::Render");
#endif
	shader_.Bind();
	SetCameraParameters(camera_);
	glBindTexture(GL_TEXTURE_2D, atlasTex_);
	const auto visibleChunks = chunkManager_.chunkStatusManager.GetVisibleChunks();
	for (auto& chunk : visibleChunks)
	{
		if (chunkManager_.chunkContentManager.GetChunkSize(chunk) == 0)
			chunkManager_.chunkStatusManager.AddStatus(chunk, ChunkFlag::EMPTY);
		else
			chunkManager_.chunkStatusManager.RemoveStatus(chunk, ChunkFlag::EMPTY);
		if (!chunkManager_.chunkStatusManager.HasStatus(chunk, ChunkFlag::LOADED) ||
			chunkManager_.chunkStatusManager.HasStatus(chunk, ChunkFlag::EMPTY) ||
			chunkManager_.chunkStatusManager.HasStatus(chunk, ChunkFlag::OCCLUDED))
			continue;
		
		shader_.SetVec3("chunkPos", Vec3f(chunkManager_.chunkPosManager.GetComponent(chunk)));
		chunkManager_.chunkRenderManager.Draw(chunk);
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
