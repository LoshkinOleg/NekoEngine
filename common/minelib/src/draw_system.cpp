#include "minelib/draw_system.h"

#include <imgui.h>

#include "minelib/aabb_manager.h"
#include "minelib/minecraft_like_engine.h"

#ifdef EASY_PROFILE_USE
#include <easy/profiler.h>
#endif

namespace neko
{
DrawSystem::DrawSystem(MinecraftLikeEngine& engine)
	: engine_(engine),
	  chunkRenderer_(engine, nullptr),
	  chunkSystem_(engine.componentsManagerSystem.chunkSystem),
	  gizmosRenderer_(nullptr),
	  playerController_(engine),
	  playerManager_(engine.componentsManagerSystem.playerManager),
	  entityViewer_(engine.entityManager, engine.entityHierarchy),
	  chunkViewer_(engine.entityManager, engine.componentsManagerSystem.chunkManager)
{
	engine.RegisterSystem(playerController_);
	engine.RegisterOnEvent(playerController_);
	engine.RegisterSystem(chunkRenderer_);
	engine.RegisterSystem(gizmosRenderer_);
}

void DrawSystem::Init()
{
	const Entity newPlayer = engine_.entityManager.CreateEntity();
	playerController_.SetCurrentPlayer(newPlayer);
	playerManager_.AddComponent(newPlayer);
	playerController_.SetCurrentPlayer(newPlayer);

	Player curPlayer = playerManager_.GetComponent(newPlayer);
	gizmosRenderer_.SetCamera(playerManager_.GetCamera(newPlayer));
	chunkRenderer_.SetCamera(playerManager_.GetCamera(newPlayer));
	curPlayer.position = Vec3f(mapSize / 2, 48, mapSize / 2);
	curPlayer.camera.position = curPlayer.position;
	playerManager_.SetComponent(newPlayer, curPlayer);
	RendererLocator::get().Render(&chunkRenderer_);
	RendererLocator::get().Render(&gizmosRenderer_);
	RendererLocator::get().Render(&engine_.uiManager);
}

void DrawSystem::Update(seconds dt)
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("DrawSystem::Update", profiler::colors::Green);
#endif
	RendererLocator::get().Render(&chunkRenderer_);
	RendererLocator::get().Render(&gizmosRenderer_);
	RendererLocator::get().Render(&engine_.uiManager);
}

void DrawSystem::Destroy()
{
	chunkRenderer_.Destroy();
	gizmosRenderer_.Destroy();
}

void DrawSystem::DrawImGui()
{
	/*
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("DrawSystem::DrawImGui", profiler::colors::Blue);
#endif
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("DrawSystem::DrawImGui::entityViewer_", profiler::colors::Blue);
#endif
	entityViewer_.DrawImGui();

#ifdef EASY_PROFILE_USE
	EASY_END_BLOCK
	EASY_BLOCK("DrawSystem::DrawImGui::Inspector", profiler::colors::Blue);
#endif
	ImGui::Begin("Inspector");
	chunkViewer_.DrawImGui(entityViewer_.GetSelectedEntity());
	ImGui::End();

	chunkRenderer_.DrawImGui();*/
}

void DrawSystem::OnEvent(const SDL_Event& event)
{
}
}
