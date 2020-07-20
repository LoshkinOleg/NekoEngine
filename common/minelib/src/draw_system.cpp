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
	camera_.Init();
	chunkRenderer_.Init();
	gizmosRenderer_.Init();
	RendererLocator::get().Render(&chunkRenderer_);
	RendererLocator::get().Render(&gizmosRenderer_);

	camera_.position = Vec3f::up * 140;
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
		rayOut_ = Ray();
		AabbLocator::get().RaycastBlock(rayOut_, camera_.position, -camera_.reverseDirection);
		savedCameraDir_ = -camera_.reverseDirection;
		savedCameraPos_ = camera_.position;
	}
	GizmosLocator::get().DrawCube(savedCameraPos_, Vec3f::one, Color4(1, 1, 1, 1));
	GizmosLocator::get().DrawCube(rayOut_.hitAabb.CalculateCenter(), Vec3f::one, Color4(1, 1, 1, 1));
	GizmosLocator::get().DrawLine(savedCameraPos_, savedCameraPos_+savedCameraDir_*10, Color4(1, 1, 1, 1));
	GizmosLocator::get().DrawLine(savedCameraPos_, rayOut_.hitAabb.CalculateCenter(), Color4(1, 1, 1, 1));
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
	if (raycastOn_)
	{
		ImGui::Begin("Testing");
		Vec3f aabbPos = rayOut_.hitAabb.CalculateCenter();
		float hitChunk = rayOut_.hitChunk;
		float hitDist = rayOut_.hitDist;
		float hitId = rayOut_.hitId;
		ImGui::InputFloat3("AabbPos", &aabbPos[0]);
		ImGui::InputFloat("hitChunk", &hitChunk);
		ImGui::InputFloat("hitDist", &hitDist);
		ImGui::InputFloat("hitId", &hitId);
		ImGui::End();
	}
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
