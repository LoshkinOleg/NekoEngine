#include "minelib/components_manager_system.h"

#include "minelib/minecraft_like_engine.h"

namespace neko
{
ComponentsManagerSystem::ComponentsManagerSystem(MinecraftLikeEngine& engine)
	: aabbManager(engine),
	  chunkSystem(engine),
	  chunkManager(engine.entityManager),
	  playerManager(engine.entityManager, engine.uiManager)
{
	engine.RegisterSystem(chunkSystem);
	engine.RegisterOnDrawUi(chunkSystem);
}

void ComponentsManagerSystem::Init()
{
}

void ComponentsManagerSystem::Update(seconds dt)
{
}

void ComponentsManagerSystem::Destroy()
{
}
}
