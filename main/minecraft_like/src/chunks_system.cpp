#include "chunks_system.h"

#include <iostream>

#include "minecraft_like_engine.h"
#include "chunks_manager.h"
#include "gizmos_renderer.h"

namespace neko
{
ChunksSystem::ChunksSystem(MinecraftLikeEngine& engine)
	: chunksManager_(engine.componentsManagerSystem_.chunkManager_),
	  transform3dManager_(engine.componentsManagerSystem_.transform3dManager_),
	  entityManager_(engine.entityManager_)
{
	Chunk chunk;
	chunksManager_.SetComponent(0, chunk);
}

void ChunksSystem::Init()
{
	transform3dManager_.SetPosition(1, Vec3f(1, 1, 1) * 5);
}

void ChunksSystem::Update(seconds dt)
{
	for (Index i = 0; i < 100; i++)
	{
		if (entityManager_.HasComponent(i, static_cast<EntityMask>(ComponentType::CHUNK)))
		{
			//GizmosLocator::get().DrawCube(transform3dManager_.GetPosition(i) + Vec3f(7.5f), Vec3f::one * 16.0f, Color4(1, 0, 0, 0.5f));
		}
	}
	GizmosLocator::get().DrawCube(transform3dManager_.GetPosition(0), Vec3f::one, Color4(1, 0, 0, 0.5f));
	GizmosLocator::get().DrawCube(transform3dManager_.GetPosition(1), Vec3f::one, Color4(1, 0, 0, 0.5f));
	GizmosLocator::get().DrawLine(transform3dManager_.GetPosition(0), transform3dManager_.GetPosition(1), Color4(0, 0, 1, 1.0f));
	
	std::cout << std::endl;
}

void ChunksSystem::Destroy()
{
}
}
