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
	  chunkManager_(engine.componentsManagerSystem.chunkManager),
	  transform3dManager_(engine.componentsManagerSystem.transform3dManager),
	  entityManager_(engine.entityManager)
{
}

void ChunkSystem::GenerateChunkArray(const Vec3i& pos) const
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("ChunkSystem::GenerateChunk");
#endif
	const Entity newChunkIndex = entityManager_.CreateEntity();
	chunkManager_.AddComponent(newChunkIndex);
	transform3dManager_.AddComponent(newChunkIndex);
				
	const auto randBlock = blockManager_.GetRandomBlock();
	chunkManager_.chunkPosManager.SetComponent(newChunkIndex, pos);
	for (uint16_t x = 0; x < kChunkSize; x++)
	{
		for (uint16_t z = 0; z < kChunkSize; z++)
		{
			chunkManager_.chunkContentManager.SetBlock(newChunkIndex, randBlock, PosToBlockId(Vec3i(x, 0, z)));
		}
	}

	chunkManager_.chunkContentManager.SetBlock(newChunkIndex, randBlock, PosToBlockId(Vec3i(8, 1, 8)));
	
	chunkManager_.chunkPosManager.SetComponent(newChunkIndex, pos);
	chunkManager_.chunkRenderManager.SetChunkValues(newChunkIndex);
	transform3dManager_.SetPosition(newChunkIndex, Vec3f(pos * kChunkSize) + Vec3f((kChunkSize - 1) / 2.0f));
}

void ChunkSystem::Init()
{
	scheduledChunks_.reserve(16);
	RendererLocator::get().Render(this);
}

void ChunkSystem::UpdateVisibleChunks()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("Chunks_System::UpdateVisibleChunks");
#endif
	//Remove last visible chunks

	const auto chunks = entityManager_.FilterEntities(static_cast<EntityMask>(ComponentType::CHUNK_CONTENT));
	const Vec3f viewerPos = GizmosLocator::get().GetCameraPos();
	const Vec3i currentChunkPos = Vec3i(std::floor(viewerPos.x / kChunkSize), 0, std::floor(viewerPos.z / kChunkSize));

	const int drawSize = kMaxViewDist_ / kChunkSize;
	for (int xOffset = -drawSize; xOffset <= drawSize; xOffset++)
	{
		for (int zOffset = -drawSize; zOffset <= drawSize;  zOffset++)
		{
			Vec3i viewedChunkPos = currentChunkPos + Vec3i(xOffset, 0, zOffset);
			bool found = false;
			for (auto chunk : chunks)
			{
				if (!entityManager_.HasComponent(chunk,
				                                 static_cast<EntityMask>(ComponentType::CHUNK_POS)))
					continue;
				
				const Vec3i chunkPos = chunkManager_.chunkPosManager.GetComponent(chunk);
				if (chunkPos == viewedChunkPos)
				{
					chunkManager_.chunkStatusManager.AddStatus(chunk, ChunkFlag::VISIBLE);
					found = true;
					break;
				}

				if (chunkPos.x > currentChunkPos.x + drawSize || 
					chunkPos.z > currentChunkPos.z + drawSize ||
					chunkPos.x < currentChunkPos.x - drawSize || 
					chunkPos.z < currentChunkPos.z - drawSize)
					chunkManager_.chunkStatusManager.RemoveStatus(chunk, ChunkFlag::VISIBLE);
			}
			
			if (!found)
			{
				scheduledChunks_.emplace_back([this, viewedChunkPos] {GenerateChunkArray(viewedChunkPos);});
			}
		}
	}
}

void ChunkSystem::Update(seconds dt)
{
	RendererLocator::get().Render(this);
	
	//Update Visible Chunks
	UpdateVisibleChunks();
	
	//Display Chunks Gizmos
	const auto loadedChunks = chunkManager_.chunkStatusManager.GetLoadedChunks();
	for (auto loadedChunk : loadedChunks)
	{
		if (entityManager_.HasComponent(loadedChunk,
		                                static_cast<EntityMask>(ComponentType::CHUNK_POS)))
		{
			GizmosLocator::get().DrawCube(
				transform3dManager_.GetPosition(loadedChunk), Vec3f::one * kChunkSize,
				chunkManager_.chunkStatusManager.HasStatus(loadedChunk, ChunkFlag::VISIBLE)
					? Color::blue : Color::red);
		}
	}
}

void ChunkSystem::Render()
{
	while (!scheduledChunks_.empty())
	{
		Job& currentTask = scheduledChunks_.front();
		currentTask.Execute();
		scheduledChunks_.erase(scheduledChunks_.begin());
	}
}

void ChunkSystem::Destroy()
{
}
}