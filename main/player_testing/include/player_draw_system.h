#pragma once
#include "player_program.h"
#include "player_engine.h"

namespace neko
{
class PlayerEngine;

class PlayerDrawSystem final : public SystemInterface, public DrawImGuiInterface, public sdl::SdlEventSystemInterface
{
public:
	explicit PlayerDrawSystem(PlayerEngine& engine);
	
	void Init() override {}
	void Update(seconds dt) override;
	void FixedUpdate() override {}
	
	void OnEvent([[maybe_unused]] const SDL_Event& event) override {}
	void DrawImGui() override;
	
	void Destroy() override;
private:
    std::mutex updateMutex_;
	
	PlayerProgram playerProgram_;
};
}
