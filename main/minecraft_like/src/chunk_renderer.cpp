#include "chunk_renderer.h"

#include "minecraft_like_engine.h"

namespace neko
{
ChunkRenderer::ChunkRenderer(
	MinecraftLikeEngine& engine,
	sdl::Camera3D& camera,
	EntityViewer& entityViewer)
	: camera_(camera),
	  engine_(engine),
	  entityViewer_(entityViewer)
{
}

void ChunkRenderer::Init()
{
	const auto& config = BasicEngine::GetInstance()->config;
	shader_.LoadFromFile(config.dataRootPath + "shaders/99_hello_scene/cube.vert",
	                     config.dataRootPath + "shaders/99_hello_scene/cube.frag");
	cube_.Init();
}

void ChunkRenderer::Update(seconds dt)
{
	timeSinceInit_ += dt;
}

void ChunkRenderer::Render()
{
	std::lock_guard<std::mutex> lock(updateMutex_);
	shader_.Bind();
	shader_.SetMat4("view", camera_.GenerateViewMatrix());
	shader_.SetMat4("projection", camera_.GenerateProjectionMatrix());
	const auto selectedEntity = entityViewer_.GetSelectedEntity();
	for (Entity entity = 0; entity < engine_.entityManager_.GetEntitiesSize(); entity++)
	{
		shader_.SetVec3("color",
		                selectedEntity == entity ? Color3(0, 1, 0) : Color3(1, 0.3f, 0.2f));
		if (engine_.entityManager_.EntityExists(entity))
		{
			const Mat4f model = engine_.componentsManagerSystem_.transform3dManager_.
			                            GetComponent(entity);
			shader_.SetMat4("model", model);
			cube_.Draw();
		}
	}
}

void ChunkRenderer::Destroy()
{
	cube_.Destroy();
	shader_.Destroy();
}
}
