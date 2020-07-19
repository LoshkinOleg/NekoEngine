#include "minelib/minecraft_like_engine.h"

namespace neko
{
MinecraftLikeEngine::MinecraftLikeEngine(Configuration* config)
	: SdlEngine(config),
	  entityHierarchy(entityManager),
	  componentsManagerSystem(*this),
	  drawSystem_(*this)
{
	RegisterSystem(drawSystem_);
	RegisterOnEvent(drawSystem_);
	RegisterOnDrawUi(drawSystem_);
	RegisterSystem(componentsManagerSystem);
	RegisterSystem(blockManager);
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