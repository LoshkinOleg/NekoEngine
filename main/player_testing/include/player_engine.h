#pragma once
#include "sdl_engine/sdl_engine.h"
#include "player_draw_system.h"

namespace neko
{
struct Configuration;

class PlayerEngine : public sdl::SdlEngine
{
public:
	explicit PlayerEngine(Configuration* config = nullptr);
	
	void Init() override;

	void GenerateUiFrame() override;

	void ManageEvent() override;

	void Destroy() override;
private:
	PlayerDrawSystem drawSystem_;
};
}
