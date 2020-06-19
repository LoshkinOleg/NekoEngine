#include "chunks_system.h"

#include <iostream>
#ifdef EASY_PROFILE_USE
#include <easy/profiler.h>
#endif
#include "engine/transform.h"


#include "aabb_manager.h"
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

void ChunksSystem::GenerateChunks()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("GenerateChunks");
#endif
	int worldSize = 3;
	for (size_t i = 0; i < worldSize*worldSize; i++)
	{
		int posZ= std::floor((i) / worldSize) - worldSize / 2;
		int posX = i % worldSize - worldSize / 2;
		transform3dManager_.SetPosition(i, Vec3f(posX*kChunkSize, 0, posZ*kChunkSize));
		Chunk chunk = chunksManager_.GetComponent(i);
		chunk.SetChunkPos(Vec3f(posX * kChunkSize, 0, posZ * kChunkSize));
		for (int x = 0; x < kChunkSize; x++)
		{
			for (int y = 0; y < kChunkSize / 2; y++)
			{
				for (int z = 0; z < kChunkSize; z++)
				{
					if (y < kChunkSize / 2 - 2)
					{
						if (RandomRange(0.0f, 1.0f) < 0.75f)
						{
							chunk.SetBlock(2, Vec3i(x, y, z));
						}
						else
						{
							chunk.SetBlock(3, Vec3i(x, y, z));
						}
					}
					else
					{
						chunk.SetBlock(1, Vec3i(x, y, z));
					}
				}
			}
		}
		chunksManager_.SetComponent(i, chunk);
		transform3dManager_.AddComponent(i);
		chunksManager_.AddComponent(i);
	}
}

void ChunksSystem::Init()
{
	GenerateChunks();
}

void ChunksSystem::Update(seconds dt)
{
	for (Index i = 0; i < INIT_ENTITY_NMB; i++)
	{
		if (entityManager_.HasComponent(i, static_cast<EntityMask>(ComponentType::CHUNK)))
		{
			GizmosLocator::get().DrawCube(transform3dManager_.GetPosition(i) + Vec3f((kChunkSize-1)/2.0f), Vec3f::one * kChunkSize, Color4(1, 0, 0, 0.5f));
		}
	}
}

void ChunksSystem::Destroy()
{
}


}
