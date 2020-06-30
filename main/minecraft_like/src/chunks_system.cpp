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
	//Chunk chunk;
	//chunksManager_.SetComponent(0, chunk);
}

void ChunksSystem::GenerateChunks()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("GenerateChunks");
#endif
	int worldSize = 10;
	for (size_t i = 0; i < worldSize * worldSize; i++)
	{
		int posZ = std::floor((i) / worldSize) - worldSize / 2;
		int posX = i % worldSize - worldSize / 2;
		transform3dManager_.SetPosition(i + 1, Vec3f(posX * kChunkSize, 0, posZ * kChunkSize));
		Chunk chunk = chunksManager_.GetComponent(i);
		chunk.SetChunkPos(Vec3i(posX, 0, posZ));
		for (int x = 0; x < kChunkSize; x++)
		{
			for (int y = 0; y < 1; y++)
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
		chunksManager_.SetComponent(i + 1, chunk);
		transform3dManager_.AddComponent(i + 1);
		chunksManager_.AddComponent(i + 1);
	}
}

Chunk ChunksSystem::GenerateChunk(const Vec3i& pos)
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("GenerateChunk");
#endif
	Chunk chunk;
	chunk.SetChunkPos(pos);
	for (int x = 0; x < kChunkSize; x++)
	{
		for (int y = 0; y < 1; y++)
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
	chunk.SetVisible(true);
	return chunk;
}

void ChunksSystem::Init()
{
	//GenerateChunks();
}

void ChunksSystem::UpdateVisibleChunks()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("Chunks_System::UpdateVisibleChunks");
#endif
	const auto visibleChunks = chunksManager_.GetVisibleChunks();
	for (auto visibleChunk : visibleChunks)
	{
		Chunk chunk = chunksManager_.GetComponent(visibleChunk);
		chunk.SetVisible(false);
		chunksManager_.SetComponent(visibleChunk, chunk);
	}
	chunksManager_.ClearVisibleChunks();
	chunksManager_.ReserveVisibleChunks((kMaxViewDist_ / kChunkSize) * (kMaxViewDist_ / kChunkSize));
	const Vec3f viewerPos = transform3dManager_.GetPosition(0);
	const Vec3i currentChunkPos = Vec3i(viewerPos.x / kChunkSize, 0, viewerPos.z/kChunkSize);

	for (int xOffset = -(kMaxViewDist_ / kChunkSize); xOffset <= (kMaxViewDist_ / kChunkSize);
	     xOffset++)
	{
		for (int zOffset = -(kMaxViewDist_ / kChunkSize); zOffset <= (kMaxViewDist_ / kChunkSize);
		     zOffset++)
		{
			Vec3i viewedChunkPos = currentChunkPos + Vec3i(xOffset, 0, zOffset);
			bool find = false;
			const auto loadedChunks = chunksManager_.GetLoadedChunks();
			for (auto loadedChunk : loadedChunks)
			{
				if (entityManager_.HasComponent(loadedChunk, static_cast<EntityMask>(ComponentType::CHUNK)))
				{
					Chunk chunk = chunksManager_.GetComponent(loadedChunk);
					if (chunk.GetChunkPos() == viewedChunkPos)
					{
						chunk.SetVisible(true);
						chunksManager_.SetComponent(loadedChunk, chunk);
						chunksManager_.AddVisibleChunk(loadedChunk);
						find = true;
						break;
					}
				}
			}
			if (!find)
			{
				Entity newChunkIndex = entityManager_.CreateEntity();
				Chunk newChunk = GenerateChunk(viewedChunkPos);
				transform3dManager_.AddComponent(newChunkIndex);
				chunksManager_.AddComponent(newChunkIndex);
				transform3dManager_.SetPosition(newChunkIndex, Vec3f(viewedChunkPos * kChunkSize));
				chunksManager_.SetComponent(newChunkIndex, newChunk);
				chunksManager_.AddLoadedChunk(newChunkIndex);
				chunksManager_.AddVisibleChunk(newChunkIndex);
			}
		}
	}
}

void ChunksSystem::Update(seconds dt)
{
	const auto loadedChunks = chunksManager_.GetLoadedChunks();
	for (auto loadedChunk : loadedChunks)
	{
		if (entityManager_.HasComponent(loadedChunk, static_cast<EntityMask>(ComponentType::CHUNK)))
		{
			GizmosLocator::get().DrawCube(
				transform3dManager_.GetPosition(loadedChunk) + Vec3f((kChunkSize - 1) / 2.0f),
				Vec3f::one * kChunkSize,
				Color4(1, 0, 0, 0.5f));
		}
	}
	UpdateVisibleChunks();
	GizmosLocator::get().DrawCube(transform3dManager_.GetPosition(0),
	                              Vec3f::one,
	                              Color4(0, 0, 1, 0.5f));
}

void ChunksSystem::Destroy()
{
}
}
