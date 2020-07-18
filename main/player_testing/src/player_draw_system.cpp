#include "player_draw_system.h"

namespace neko
{
PlayerDrawSystem::PlayerDrawSystem(PlayerEngine& engine) : playerProgram_(engine)
{
	engine.RegisterSystem(playerProgram_);
	engine.RegisterOnEvent(playerProgram_);
}

void PlayerDrawSystem::Update(seconds dt)
{
	std::lock_guard<std::mutex> lock(updateMutex_);
	RendererLocator::get().Render(&playerProgram_);
}

void PlayerDrawSystem::DrawImGui()
{
	std::lock_guard<std::mutex> lock(updateMutex_);
	playerProgram_.DrawImGui();
}

void PlayerDrawSystem::Destroy()
{
	playerProgram_.Destroy();
}
}
