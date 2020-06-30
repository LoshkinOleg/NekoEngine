#include "minecraft_like_engine.h"
#ifdef EASY_PROFILE_USE
#include <easy/profiler.h>
#endif

namespace neko
{
MinecraftLikeEngine::MinecraftLikeEngine(Configuration* config)
	: SdlEngine(config),
	  entityManager_(),
	  entityHierarchy_(entityManager_),
	  componentsManagerSystem_(*this),
	  drawSystem_(*this)
{
	RegisterSystem(drawSystem_);
	RegisterOnEvent(drawSystem_);
	RegisterOnDrawUi(drawSystem_);
	RegisterSystem(componentsManagerSystem_);
#ifdef EASY_PROFILE_USE
	EASY_PROFILER_ENABLE;
#endif

}

void MinecraftLikeEngine::Init()
{
	SdlEngine::Init();
	//Temporary scene init
    const auto entity = entityManager_.CreateEntity();
	componentsManagerSystem_.transform3dManager_.AddComponent(entity);
	for (int i = 0; i < kInitEntityNmb_; i++)
	{
	}
}

void MinecraftLikeEngine::Destroy()
{
#ifdef EASY_PROFILE_USE
	profiler::dumpBlocksToFile("minecraft_profile.prof");
#endif
	drawSystem_.Destroy();
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