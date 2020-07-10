#include "minelib/components_manager_system.h"

#include "minelib/minecraft_like_engine.h"

namespace neko
{
ComponentsManagerSystem::ComponentsManagerSystem(MinecraftLikeEngine& engine)
	: transform3dManager(engine.entityManager),
	  aabbManager(engine),
	  chunkContentManager(engine.entityManager),
	  chunkPosManager(engine.entityManager),
	  chunkRenderManager(engine.entityManager, chunkContentManager),
	  chunkStatusManager(engine.entityManager),
	  chunksSystem_(engine)
{
	engine.RegisterSystem(chunksSystem_);
}

void ComponentsManagerSystem::Init()
{
	transform3dManager.Init();
}

void ComponentsManagerSystem::Update(seconds dt)
{
	transform3dManager.Update();
}

void ComponentsManagerSystem::Destroy()
{
}
}
