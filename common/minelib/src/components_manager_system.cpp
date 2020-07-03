#include "minelib/components_manager_system.h"

#include "minelib/minecraft_like_engine.h"

namespace neko
{
ComponentsManagerSystem::ComponentsManagerSystem(MinecraftLikeEngine& engine)
	: transform3dManager_(engine.entityManager_),
	  chunkManager_(engine.entityManager_),
	  aabbManager_(engine),
	  chunksSystem_(engine)
{
	engine.RegisterSystem(chunksSystem_);
}

void ComponentsManagerSystem::Init()
{
	transform3dManager_.Init();
}

void ComponentsManagerSystem::Update(seconds dt)
{
	transform3dManager_.Update();
}

void ComponentsManagerSystem::Destroy()
{
}
}
