#include "draw_system.h"

#include <imgui.h>


#include "aabb_manager.h"
#include "minecraft_like_engine.h"

namespace neko
{
DrawSystem::DrawSystem(MinecraftLikeEngine& engine)
	: chunkRenderer_(engine, camera_, entityViewer_),
	  gizmosRenderer_(engine, camera_),
	  entityViewer_(engine.entityManager_, engine.entityHierarchy_),
	  transformViewer_(engine.entityManager_, engine.componentsManagerSystem_.transform3dManager_),
	  chunksViewer_(engine.entityManager_, engine.componentsManagerSystem_.chunkManager_),
	  engine_(engine)
{
	engine.RegisterSystem(camera_);
	engine.RegisterOnEvent(camera_);
}

void DrawSystem::Init()
{
	camera_.Init();
	chunkRenderer_.Init();
	gizmosRenderer_.Init();
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

	ImGui::Begin("Light");
	
	ImGui::InputFloat3("Position", &engine_.componentsManagerSystem_.light_.lightPos_[0]);
	ImGui::InputFloat3("Direction", &engine_.componentsManagerSystem_.light_.lightDirection_[0]);
	ImGui::End();
}

void DrawSystem::Update(seconds dt)
{
	chunkRenderer_.Update(dt);
	gizmosRenderer_.Update(dt);
	RendererLocator::get().Render(&chunkRenderer_);
	RendererLocator::get().Render(&gizmosRenderer_);
	//if (sdl::InputLocator::get().IsKeyDown(sdl::KeyCode::TAB))
	{
		Block block = AabbLocator::get().RaycastBlock(camera_.position, -camera_.reverseDirection);
		savedCameraDir_ = -camera_.reverseDirection;
		savedCameraPos_ = camera_.position;
		/*std::cout << "BlockPos : " << block.blockPos << std::endl;
		std::cout << "BlockID : " << static_cast<int>(block.blockType) << std::endl;*/
		GizmosLocator::get().DrawCube(block.blockPos, Vec3f::one, Color4(1,1,1,1));
	}
	GizmosLocator::get().DrawLine(savedCameraPos_, savedCameraPos_ + savedCameraDir_*10);
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