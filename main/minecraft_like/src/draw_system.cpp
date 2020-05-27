#include "draw_system.h"

#include <imgui.h>

#include "minecraft_like_engine.h"

namespace neko
{
DrawSystem::DrawSystem(MinecraftLikeEngine& engine)
	: chunkRenderer_(engine, camera_, entityViewer_),
	  entityViewer_(engine.entityManager_, engine.entityHierarchy_),
	  transformViewer_(engine.entityManager_, engine.componentsManagerSystem_.transform3dManager_),
	  engine_(engine)
{
}

void DrawSystem::Init()
{
	camera_.Init();
	chunkRenderer_.Init();
}

void DrawSystem::DrawImGui()
{
	std::lock_guard<std::mutex> lock(updateMutex_);
	entityViewer_.DrawImGui();
	transformViewer_.SetSelectedEntity(entityViewer_.GetSelectedEntity());
	ImGui::Begin("Inspector");
	transformViewer_.DrawImGui();
	ImGui::End();
}

void DrawSystem::Update(seconds dt)
{
	chunkRenderer_.Update(dt);
	RendererLocator::get().Render(&chunkRenderer_);
	std::lock_guard<std::mutex> lock(updateMutex_);
	const auto& config = BasicEngine::GetInstance()->config;
	camera_.SetAspect(config.windowSize.x, config.windowSize.y);
	camera_.Update(dt);
}

void DrawSystem::Destroy()
{
	chunkRenderer_.Destroy();
}

void DrawSystem::OnEvent(const SDL_Event& event)
{
	camera_.OnEvent(event);
}
}
