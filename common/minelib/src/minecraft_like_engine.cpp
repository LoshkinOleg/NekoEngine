#include "minelib/minecraft_like_engine.h"

namespace neko
{ 
MinecraftLikeEngine::MinecraftLikeEngine(Configuration* config)
	: SdlEngine(config),
	  entityHierarchy(entityManager),
	  componentsManagerSystem(*this),
	  drawSystem_(*this)
{
	RegisterSystem(componentsManagerSystem);
	RegisterSystem(blockManager);
	RegisterSystem(drawSystem_);
	RegisterOnEvent(drawSystem_);
	RegisterOnDrawUi(drawSystem_);
	RegisterSystem(uiManager);
	RegisterOnEvent(uiManager);
#ifdef EASY_PROFILE_USE
	EASY_PROFILER_ENABLE;
#endif
}

void MinecraftLikeEngine::Init()
{
	SdlEngine::Init();
}

void MinecraftLikeEngine::Destroy()
{
	drawSystem_.Destroy();
	componentsManagerSystem.Destroy();
	blockManager.Destroy();
	SdlEngine::Destroy();
}

void MinecraftLikeEngine::ManageEvent()
{
	SdlEngine::ManageEvent();
}

void MinecraftLikeEngine::GenerateUiFrame()
{
	SdlEngine::GenerateUiFrame();
}
}