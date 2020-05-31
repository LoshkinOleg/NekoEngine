#include "chunk_renderer.h"

#include <graphics/camera.h>


#include "gl/texture.h"
#include "minecraft_like_engine.h"

namespace neko
{
ChunkRenderer::ChunkRenderer(
	MinecraftLikeEngine& engine,
	MoveableCamera3D& camera,
	EntityViewer& entityViewer)
	: camera_(camera)
{
}

void ChunkRenderer::Init()
{
	const auto& config = BasicEngine::GetInstance()->config;
	shader_.LoadFromFile(
		config.dataRootPath + "shaders/base.vert",
		config.dataRootPath + "shaders/base.frag");
	texture_ = gl::stbCreateTexture(config.dataRootPath + "sprites/wall.jpg");
	cube_.Init();

	camera_.Init();
	camera_.position = Vec3f::forward * 3;

	glEnable(GL_DEPTH_TEST);
}

void ChunkRenderer::Update(seconds dt)
{
	std::lock_guard<std::mutex> lock(updateMutex_);

	camera_.Update(dt);
}

void ChunkRenderer::Render()
{
	if (shader_.GetProgram() == 0) return;
	if (texture_ == INVALID_TEXTURE_ID) return;

	std::lock_guard<std::mutex> lock(updateMutex_);

	shader_.Bind();
	glBindTexture(GL_TEXTURE_2D, texture_); //bind texture id to texture slot
	shader_.SetMat4("view", camera_.GenerateViewMatrix());
	shader_.SetMat4("projection", camera_.GenerateProjectionMatrix());
	shader_.SetMat4("model", Mat4f::Identity);

	cube_.Draw();
}

void ChunkRenderer::Destroy()
{
	cube_.Destroy();
	shader_.Destroy();
	gl::DestroyTexture(texture_);
}

void ChunkRenderer::DrawImGui()
{
}

void ChunkRenderer::OnEvent(const SDL_Event& event)
{
	camera_.OnEvent(event);
}
}
