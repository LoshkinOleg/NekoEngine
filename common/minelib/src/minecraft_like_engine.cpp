#include "minelib/minecraft_like_engine.h"
#ifdef EASY_PROFILE_USE
#include <easy/profiler.h>
#endif

namespace neko
{
MinecraftLikeEngine::MinecraftLikeEngine(Configuration* config)
	: SdlEngine(config),
	  entityHierarchy(entityManager),
	  componentsManagerSystem(*this),
	  drawSystem_(*this)
{
	RegisterSystem(uiManager);
	RegisterOnEvent(uiManager);
	RegisterSystem(componentsManagerSystem);
	RegisterSystem(blockManager);
	RegisterSystem(drawSystem_);
	RegisterOnEvent(drawSystem_);
	RegisterOnDrawUi(drawSystem_);
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
#ifdef EASY_PROFILE_USE
	profiler::dumpBlocksToFile("minecraft_profile.prof");
#endif
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