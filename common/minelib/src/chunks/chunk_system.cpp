#include "minelib/chunks/chunk_system.h"

#ifdef EASY_PROFILE_USE
#include <easy/profiler.h>
#endif
#include "engine/transform.h"

#include <minelib/chunks/chunk_manager.h>
#include "minelib/gizmos_renderer.h"
#include "minelib/minecraft_like_engine.h"

namespace neko
{
ChunkSystem::ChunkSystem(MinecraftLikeEngine& engine)
	: blockManager_(engine.blockManager),
	  chunkContentManager_(engine.componentsManagerSystem.chunkContentManager),
	  chunkStatusManager_(engine.componentsManagerSystem.chunkStatusManager),
	  chunkPosManager_(engine.componentsManagerSystem.chunkPosManager),
	  chunkRenderManager_(engine.componentsManagerSystem.chunkRenderManager),
	  transform3dManager_(engine.componentsManagerSystem.transform3dManager),
	  entityManager_(engine.entityManager)
{
}

void ChunkSystem::GenerateChunkArray(const Entity index, const Vec3i& pos) const
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("ChunkSystem::GenerateChunk");
#endif
	ChunkContentVector blocksArray;
	const auto randBlock = blockManager_.GetRandomBlock();
	for (uint16_t x = 0; x < kChunkSize; x++)
	{
		for (uint16_t y = 0; y < 1; y++)
		{
			for (uint16_t z = 0; z < kChunkSize; z++)
			{
				chunkContentManager_.SetBlock(index, randBlock, PosToBlockId(Vec3i(x, y, z)));
			}
		}
	}
}

void ChunkSystem::Init()
{
}

void ChunkSystem::UpdateVisibleChunks() const
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("Chunks_System::UpdateVisibleChunks");
#endif
	//Remove last visible chunks

	const auto chunksNmb = entityManager_.GetEntitiesSize();
	for (Index chunkIndex = 0; chunkIndex < chunksNmb; chunkIndex++)
	{
		if (!entityManager_.HasComponent(chunkIndex,
		                                 static_cast<EntityMask>(ComponentType::CHUNK_POS)))
			continue;
		chunkStatusManager_.RemoveStatus(chunkIndex, ChunkFlag::VISIBLE);
	}

	const Vec3f viewerPos = transform3dManager_.GetPosition(0);
	const Vec3i currentChunkPos = Vec3i(std::floor(viewerPos.x / kChunkSize), 0, std::floor(viewerPos.z / kChunkSize));
	
	for (int xOffset = -(kMaxViewDist_ / kChunkSize); xOffset <= (kMaxViewDist_ / kChunkSize);
	     xOffset++)
	{
		for (int zOffset = -(kMaxViewDist_ / kChunkSize); zOffset <= (kMaxViewDist_ / kChunkSize);
		     zOffset++)
		{
			Vec3i viewedChunkPos = currentChunkPos + Vec3i(xOffset, 0, zOffset);
			bool found = false;
			for (Index chunkIndex = 0; chunkIndex < chunksNmb; chunkIndex++)
			{
				if (!entityManager_.HasComponent(chunkIndex,
				                                 static_cast<EntityMask>(ComponentType::CHUNK_POS)))
					continue;
				Vec3i chunkPos = chunkPosManager_.GetComponent(chunkIndex);
				if (chunkPos == viewedChunkPos)
				{
					chunkStatusManager_.AddStatus(chunkIndex, ChunkFlag::VISIBLE);
					found = true;
					break;
				}
			}
			if (!found)
			{
				const Entity newChunkIndex = entityManager_.CreateEntity();
				chunkPosManager_.AddComponent(newChunkIndex);
				chunkStatusManager_.AddComponent(newChunkIndex);
				chunkContentManager_.AddComponent(newChunkIndex);
				chunkRenderManager_.AddComponent(newChunkIndex);
				transform3dManager_.AddComponent(newChunkIndex);
				
				GenerateChunkArray(newChunkIndex, viewedChunkPos);
				chunkPosManager_.SetComponent(newChunkIndex, viewedChunkPos);
				chunkStatusManager_.AddStatus(newChunkIndex, ChunkFlag::VISIBLE);
				chunkStatusManager_.AddStatus(newChunkIndex, ChunkFlag::LOADED);
				transform3dManager_.SetPosition(newChunkIndex, Vec3f(viewedChunkPos * kChunkSize));
			}
		}
	}
}

void ChunkSystem::Update(seconds dt)
{
	//Update Visible Chunks
	UpdateVisibleChunks();

	//Display Chunks Gizmos
	const auto loadedChunks = chunkStatusManager_.GetLoadedChunks();
	for (auto loadedChunk : loadedChunks)
	{
		if (entityManager_.HasComponent(loadedChunk,
		                                static_cast<EntityMask>(ComponentType::CHUNK_POS)))
		{
			GizmosLocator::get().DrawCube(
				transform3dManager_.GetPosition(loadedChunk) + Vec3f::one * kChunkSize / 2 -
				Vec3f::one / 2,
				Vec3f::one * kChunkSize,
				chunkStatusManager_.HasStatus(loadedChunk, ChunkFlag::VISIBLE)
					? Color4(1.0f, 0.0f, 0.0f, 1.0f)
					: Color4(0.0f, 0.0f, 1.0f, 1.0f));
		}
	}
}

void ChunkSystem::Destroy()
{
}
}
