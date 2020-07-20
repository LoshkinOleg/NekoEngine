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
	RendererLocator::get().Render(&chunkRenderer_);
	RendererLocator::get().Render(&gizmosRenderer_);

	camera_.position = Vec3f::up * 32;
}

void DrawSystem::Update(seconds dt)
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("DrawSystem::Update", profiler::colors::Green);
#endif
	RendererLocator::get().Render(&chunkRenderer_);
	RendererLocator::get().Render(&gizmosRenderer_);
	if (raycastOn_)
	{
		Ray rayOut;
		AabbLocator::get().RaycastBlock(rayOut, camera_.position, -camera_.reverseDirection);
		savedCameraDir_ = -camera_.reverseDirection;
		savedCameraPos_ = camera_.position;
		GizmosLocator::get().DrawCube(rayOut.hitAabb.CalculateCenter(), Vec3f::one, Color4(1, 1, 1, 1));
	}
}

void DrawSystem::Destroy()
{
	chunkRenderer_.Destroy();
	gizmosRenderer_.Destroy();
}

void DrawSystem::DrawImGui()
{
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

#ifdef EASY_PROFILE_USE
	EASY_END_BLOCK
		EASY_BLOCK("DrawSystem::DrawImGui::Testing", profiler::colors::Blue);
#endif
	ImGui::Begin("Testing");
	ImGui::Checkbox("Raycast", &raycastOn_);
	if (ImGui::Checkbox("Gizmos", &gizmosOn_))
	{
		if (gizmosOn_)
		{
			gizmosRenderer_.Start();
		} else
		{
			gizmosRenderer_.Stop();
		}
	}
	
	ImGui::End();

#ifdef EASY_PROFILE_USE
	EASY_END_BLOCK
		EASY_BLOCK("DrawSystem::DrawImGui::chunkRenderer_", profiler::colors::Blue);
#endif
	chunkRenderer_.DrawImGui();
}

void DrawSystem::OnEvent(const SDL_Event& event)
{
}
}
