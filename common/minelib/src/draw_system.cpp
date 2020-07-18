#include "minelib/draw_system.h"

#include <imgui.h>

#include "minelib/aabb_manager.h"
#include "minelib/minecraft_like_engine.h"

namespace neko
{
DrawSystem::DrawSystem(MinecraftLikeEngine& engine)
	: engine_(engine),
	  chunkRenderer_(engine, camera_),
	  chunkSystem_(engine.componentsManagerSystem.chunkSystem),
	  gizmosRenderer_(camera_),
	  entityViewer_(engine.entityManager, engine.entityHierarchy),
	  chunkViewer_(engine.entityManager, engine.componentsManagerSystem.chunkManager)
{
	engine.RegisterSystem(camera_);
	engine.RegisterOnEvent(camera_);
	engine.RegisterSystem(chunkRenderer_);
	engine.RegisterSystem(gizmosRenderer_);
}

void DrawSystem::Init()
{
	camera_.Init();
	chunkRenderer_.Init();
	gizmosRenderer_.Init();
	RendererLocator::get().Render(&chunkRenderer_);
	RendererLocator::get().Render(&gizmosRenderer_);

	camera_.position = Vec3f::up * 140;
}

void DrawSystem::Update(seconds dt)
{
	RendererLocator::get().Render(&chunkRenderer_);
	RendererLocator::get().Render(&gizmosRenderer_);
	//if (sdl::InputLocator::get().IsKeyDown(sdl::KeyCode::TAB))
	{
		Ray rayOut;
		AabbLocator::get().RaycastBlock(rayOut, camera_.position, -camera_.reverseDirection);
		savedCameraDir_ = -camera_.reverseDirection;
		savedCameraPos_ = camera_.position;
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