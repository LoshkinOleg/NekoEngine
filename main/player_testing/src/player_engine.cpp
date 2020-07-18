#include "player_engine.h"

namespace neko
{
PlayerEngine::PlayerEngine(Configuration* config) : SdlEngine(config), drawSystem_(*this)
{
	RegisterSystem(drawSystem_);
	RegisterOnDrawUi(drawSystem_);
	RegisterOnEvent(drawSystem_);
}

void PlayerEngine::Init()
{
	SdlEngine::Init();
}

void PlayerEngine::GenerateUiFrame()
{
	SdlEngine::GenerateUiFrame();
}

void PlayerEngine::ManageEvent()
{
	SdlEngine::ManageEvent();
}

void PlayerEngine::Destroy()
{
	drawSystem_.Destroy();
	SdlEngine::Destroy();
}
}
