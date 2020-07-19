#include "minelib/draw_system.h"

#include <imgui.h>

#include "minelib/aabb_manager.h"
#include "minelib/minecraft_like_engine.h"

namespace neko
{
DrawSystem::DrawSystem(MinecraftLikeEngine& engine)
	: engine_(engine),
	  chunkRenderer_(engine, Camera3D()),
	  chunkSystem_(engine.componentsManagerSystem.chunkSystem),
	  gizmosRenderer_(Camera3D()),
	  playerController_(engine),
	  playerManager_(engine.componentsManagerSystem.playerManager),
	  entityViewer_(engine.entityManager, engine.entityHierarchy),
	  chunkViewer_(engine.entityManager, engine.componentsManagerSystem.chunkManager)
{
	engine.RegisterSystem(chunkRenderer_);
	engine.RegisterSystem(gizmosRenderer_);
	engine.RegisterSystem(playerController_);
	engine.RegisterOnEvent(playerController_);
}

void DrawSystem::Init()
{
	chunkRenderer_.Init();
	gizmosRenderer_.Init();
	RendererLocator::get().Render(&chunkRenderer_);
	RendererLocator::get().Render(&gizmosRenderer_);
	
	const Entity newPlayer = engine_.entityManager.CreateEntity();
	playerController_.SetCurrentPlayer(newPlayer);
	playerManager_.AddComponent(newPlayer);
	playerController_.SetCurrentPlayer(newPlayer);

	Player& curPlayer = playerManager_.GetComponent(newPlayer);
	gizmosRenderer_.SetCamera(curPlayer.camera);
	chunkRenderer_.SetCamera(curPlayer.camera);

	curPlayer.position = Vec3f::up * 140;
	curPlayer.camera.position = curPlayer.position;
	playerManager_.SetComponent(newPlayer, curPlayer);
}

void DrawSystem::Update(seconds dt)
{
	RendererLocator::get().Render(&chunkRenderer_);
	RendererLocator::get().Render(&gizmosRenderer_);
	//if (sdl::InputLocator::get().IsKeyDown(sdl::KeyCode::TAB))
	{
		Ray rayOut;
		Player& curPlayer = playerManager_.GetComponent(playerController_.GetCurrentPlayer());
		AabbLocator::get().RaycastBlock(rayOut, curPlayer.camera.position, -curPlayer.camera.reverseDirection);
		savedCameraDir_ = -curPlayer.camera.reverseDirection;
		savedCameraPos_ = curPlayer.camera.position;
		GizmosLocator::get().DrawCube(rayOut.hitAabb.CalculateCenter(), Vec3f::one, Color4(1, 1, 1, 1));
	}
	GizmosLocator::get().DrawLine(savedCameraPos_, savedCameraPos_ + savedCameraDir_ * 10);
}

void DrawSystem::Destroy()
{
	chunkRenderer_.Destroy();
	gizmosRenderer_.Destroy();
}

void DrawSystem::DrawImGui()
{
	entityViewer_.DrawImGui();
	
	ImGui::Begin("Inspector");
	chunkViewer_.DrawImGui(entityViewer_.GetSelectedEntity());
	ImGui::End();

	chunkRenderer_.DrawImGui();
}

void DrawSystem::OnEvent(const SDL_Event& event)
{
}
}
