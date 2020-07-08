#include "minelib/draw_system.h"

#include <imgui.h>

#include "minelib/aabb_manager.h"
#include "minelib/minecraft_like_engine.h"

namespace neko
{
DrawSystem::DrawSystem(MinecraftLikeEngine& engine)
	: chunkRenderer_(engine, camera_, entityViewer_),
	  gizmosRenderer_(camera_),
	  entityViewer_(engine.entityManager, engine.entityHierarchy),
	  transformViewer_(engine.entityManager, engine.componentsManagerSystem.transform3dManager),
	  chunksViewer_(engine.entityManager,
	                engine.componentsManagerSystem.chunkContentManager,
	                engine.componentsManagerSystem.chunkStatutManager,
	                engine.componentsManagerSystem.chunkPosManager),
	  engine_(engine)
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

	camera_.position = Vec3f::up;
}

void DrawSystem::DrawImGui()
{
	std::lock_guard<std::mutex> lock(updateMutex_);
	entityViewer_.DrawImGui();
	transformViewer_.SetSelectedEntity(entityViewer_.GetSelectedEntity());
	ImGui::Begin("Inspector");
	transformViewer_.DrawImGui();
	chunksViewer_.DrawImGui(entityViewer_.GetSelectedEntity());
	ImGui::End();
	ImGui::Begin("Player");
	Vec3f blockPos = viewBlock_.blockPos;
	int blockId = viewBlock_.blockType;
	ImGui::InputFloat3("BlockPos", &blockPos[0]);
	ImGui::DragInt("BlockId", &blockId);
	ImGui::End();

	chunkRenderer_.DrawImGui();
}

void DrawSystem::Update(seconds dt)
{
	RendererLocator::get().Render(&chunkRenderer_);
	RendererLocator::get().Render(&gizmosRenderer_);
	//if (sdl::InputLocator::get().IsKeyDown(sdl::KeyCode::TAB))
	{
		/*viewBlock_ = AabbLocator::get().RaycastBlock(camera_.position, -camera_.reverseDirection);
		savedCameraDir_ = -camera_.reverseDirection;
		savedCameraPos_ = camera_.position;
		GizmosLocator::get().DrawCube(viewBlock_.blockPos, Vec3f::one, Color4(1, 1, 1, 1));*/
	}
	GizmosLocator::get().DrawLine(savedCameraPos_, savedCameraPos_ + savedCameraDir_ * 10);
}

void DrawSystem::Destroy()
{
	chunkRenderer_.Destroy();
	gizmosRenderer_.Destroy();
}

void DrawSystem::OnEvent(const SDL_Event& event)
{
}
}
