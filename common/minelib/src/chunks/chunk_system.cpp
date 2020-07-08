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
ChunksSystem::ChunksSystem(MinecraftLikeEngine& engine)
	: chunkStatutManager_(engine.componentsManagerSystem.chunkStatutManager),
	  chunkContentManager_(engine.componentsManagerSystem.chunkContentManager),
	  chunkPosManager_(engine.componentsManagerSystem.chunkPosManager),
	  transform3dManager_(engine.componentsManagerSystem.transform3dManager),
	  entityManager_(engine.entityManager)
{
}

void ChunksSystem::GenerateChunkArray(const Entity index, const Vec3i& pos) const
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("ChunksSystem::GenerateChunk");
#endif
	BlocksArray blocksArray;
	const uint8_t randBlockId = RandomRange(1, 4);
	for (unsigned x = 0; x < kChunkSize; x++)
	{
		for (unsigned y = 0; y < 1; y++)
		{
			for (unsigned z = 0; z < kChunkSize; z++)
			{
				//if (y < kChunkSize / 2 - 2)
				//{
				//	if (RandomRange(0.0f, 1.0f) < 0.75f)
				//	{
				//		chunk.SetBlock(2, Vec3i(x, y, z));
				//	}
				//	else
				//	{
				//		chunk.SetBlock(3, Vec3i(x, y, z));
				//	}
				//}
				//else
				{
					chunkContentManager_.SetBlock(index, randBlockId, Vec3i(x, y, z));
				}
			}
		}
	}
}

void ChunksSystem::Init()
{
}

void ChunksSystem::UpdateVisibleChunks() const
{
	const unsigned int maxViewChunks = (kMaxViewDist_ / kChunkSize);
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
		chunkStatutManager_.RemoveStatut(chunkIndex, ChunkFlag::VISIBLE);
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
			const auto chunksNmb = entityManager_.GetEntitiesSize();
			for (Index chunkIndex = 0; chunkIndex < chunksNmb; chunkIndex++)
			{
				if (!entityManager_.HasComponent(chunkIndex,
				                                 static_cast<EntityMask>(ComponentType::CHUNK_POS)))
					continue;
				Vec3i chunkPos = chunkPosManager_.GetComponent(chunkIndex);
				if (chunkPos == viewedChunkPos)
				{
					chunkStatutManager_.AddStatut(chunkIndex, ChunkFlag::VISIBLE);
					found = true;
					break;
				}
			}
			if (!found)
			{
				const Entity newChunkIndex = entityManager_.CreateEntity();
				chunkPosManager_.AddComponent(newChunkIndex);
				chunkStatutManager_.AddComponent(newChunkIndex);
				chunkContentManager_.AddComponent(newChunkIndex);
				transform3dManager_.AddComponent(newChunkIndex);
				GenerateChunkArray(newChunkIndex, viewedChunkPos);
				chunkPosManager_.SetComponent(newChunkIndex, viewedChunkPos);
				chunkStatutManager_.AddStatut(newChunkIndex, ChunkFlag::VISIBLE);
				chunkStatutManager_.AddStatut(newChunkIndex, ChunkFlag::LOADED);
				transform3dManager_.SetPosition(newChunkIndex, Vec3f(viewedChunkPos * kChunkSize));
			}
		}
	}
}

void ChunksSystem::Update(seconds dt)
{
	//Update Visible Chunks
	UpdateVisibleChunks();

	//Display Chunks Gizmos
	const Vec3f cubeOffset = Vec3f((kChunkSize - 1) / 2.0f);
	const auto loadedChunks = chunkStatutManager_.GetLoadedChunks();
	for (auto loadedChunk : loadedChunks)
	{
		if (entityManager_.HasComponent(loadedChunk,
		                                static_cast<EntityMask>(ComponentType::CHUNK_POS)))
		{
			GizmosLocator::get().DrawCube(
				transform3dManager_.GetPosition(loadedChunk) + Vec3f::one * kChunkSize / 2 -
				Vec3f::one / 2,
				Vec3f::one * kChunkSize,
				chunkStatutManager_.HasStatut(loadedChunk, ChunkFlag::VISIBLE)
					? Color4(1.0f, 0.0f, 0.0f, 1.0f)
					: Color4(0.0f, 0.0f, 1.0f, 1.0f));
		}
	}
}

void ChunksSystem::Destroy()
{
}
}
