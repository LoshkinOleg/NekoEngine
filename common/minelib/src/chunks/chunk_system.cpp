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

void ChunkSystem::GenerateChunkArray(const Vec3i& pos)
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("ChunkSystem::GenerateChunk");
#endif
	const Entity newChunkIndex = entityManager_.CreateEntity();
	chunkManager_.AddComponent(newChunkIndex);
	transform3dManager_.AddComponent(newChunkIndex);


	const auto randBlock = blockManager_.GetRandomBlock();
	chunkManager_.chunkPosManager.SetComponent(newChunkIndex, pos);
	if (pos.y > kHeighChunktLimit_ / 2)
	{
		
	} else if (pos.y < kHeighChunktLimit_/2)
	{
		for (uint16_t x = 0; x < kChunkSize; x++)
		{
			for (uint16_t y = 0; y < kChunkSize; y++)
			{
				for (uint16_t z = 0; z < kChunkSize; z++)
				{
					chunkManager_.chunkContentManager.SetBlock(newChunkIndex, randBlock, PosToBlockId(Vec3i(x, y, z)));
				}
			}
		}
		chunkManager_.chunkStatusManager.AddStatus(newChunkIndex, ChunkFlag::OCCLUDE_DOWN);
		chunkManager_.chunkStatusManager.AddStatus(newChunkIndex, ChunkFlag::OCCLUDE_UP);
		chunkManager_.chunkStatusManager.AddStatus(newChunkIndex, ChunkFlag::OCCLUDE_LEFT);
		chunkManager_.chunkStatusManager.AddStatus(newChunkIndex, ChunkFlag::OCCLUDE_RIGHT);
		chunkManager_.chunkStatusManager.AddStatus(newChunkIndex, ChunkFlag::OCCLUDE_FRONT);
		chunkManager_.chunkStatusManager.AddStatus(newChunkIndex, ChunkFlag::OCCLUDE_BACK);
	} else
	{
		for (uint16_t x = 0; x < kChunkSize; x++)
		{
			for (uint16_t z = 0; z < kChunkSize; z++)
			{
				chunkManager_.chunkContentManager.SetBlock(newChunkIndex, randBlock, PosToBlockId(Vec3i(x, 0, z)));
			}
		}
		chunkManager_.chunkStatusManager.AddStatus(newChunkIndex, ChunkFlag::OCCLUDE_DOWN);
	}
#ifdef EASY_PROFILE_USE
	EASY_END_BLOCK
#endif

	chunkManager_.chunkRenderManager.SetChunkValues(newChunkIndex);
	transform3dManager_.SetPosition(newChunkIndex, Vec3f(pos * kChunkSize) + Vec3f((kChunkSize - 1) / 2.0f));
}

void ChunkSystem::Init()
{
	std::lock_guard<std::mutex> lock(mutex_);
	scheduledChunks_.reserve(16);
	RendererLocator::get().Render(this);
}

void ChunkSystem::SetChunkOcclusionCulling(const Entity chunkIndex)
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("Chunks_System::SetChunkOcclusionCulling");
#endif
	Vec3i pos = chunkManager_.chunkPosManager.GetComponent(chunkIndex);
	bool visible = false;
	const auto chunks = entityManager_.FilterEntities(static_cast<EntityMask>(ComponentType::CHUNK_STATUS));
	for (auto chunk : chunks)
	{
		if (!entityManager_.HasComponent(chunk,
			static_cast<EntityMask>(ComponentType::CHUNK_POS)))
			continue;

		const Vec3i chunkPos = chunkManager_.chunkPosManager.GetComponent(chunk);
		for (std::uint16_t occlude = static_cast<std::uint16_t>(ChunkFlag::OCCLUDE_DOWN); occlude <= static_cast<std::uint16_t>(ChunkFlag::OCCLUDE_BACK); occlude = occlude << 1u)
		{
			Vec3i offset;
			switch (static_cast<ChunkFlag>(occlude)) {
				case ChunkFlag::OCCLUDE_DOWN:
					offset = Vec3i::up;
				break;
				case ChunkFlag::OCCLUDE_UP:
					offset = Vec3i::down;
				break;
				case ChunkFlag::OCCLUDE_RIGHT:
					offset = Vec3i::left;
				break;
				case ChunkFlag::OCCLUDE_LEFT:
					offset = Vec3i::right;
				break;
				case ChunkFlag::OCCLUDE_FRONT:
					offset = Vec3i::back;
				break;
				case ChunkFlag::OCCLUDE_BACK:
					offset = Vec3i::forward;
				break;
				default: ;
			}
			if (chunkPos == pos + offset)
			{
				if (chunkManager_.chunkStatusManager.HasStatus(chunk, ChunkFlag::VISIBLE) && !chunkManager_.chunkStatusManager.HasStatus(chunk, static_cast<ChunkFlag>(occlude)))
				{
					visible = true;
					break;
				}
			}
		}
		if (visible) break;
	}
	if (!visible)
	{
		chunkManager_.chunkStatusManager.RemoveStatus(chunkIndex, ChunkFlag::VISIBLE);
	}
}

void ChunkSystem::UpdateVisibleChunks()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("Chunks_System::UpdateVisibleChunks");
#endif
	//Remove last visible chunks

	std::lock_guard<std::mutex> lock(mutex_);
	const auto chunks = entityManager_.FilterEntities(static_cast<EntityMask>(ComponentType::CHUNK_CONTENT));
	const Vec3f viewerPos = GizmosLocator::get().GetCameraPos();
	const Vec3i currentChunkPos = Vec3i(std::floor(viewerPos.x / kChunkSize), 0, std::floor(viewerPos.z / kChunkSize));

	const int drawSize = kMaxViewDist_ / kChunkSize;
	for (int xOffset = -drawSize; xOffset <= drawSize; xOffset++)
	{
		for (int zOffset = -drawSize; zOffset <= drawSize; zOffset++)
		{
			for (int yOffset = 0; yOffset < kHeighChunktLimit_; yOffset++)
			{
				Vec3i viewedChunkPos = currentChunkPos + Vec3i(xOffset, yOffset, zOffset);
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
					scheduledChunks_.emplace_back([this, viewedChunkPos] {GenerateChunkArray(viewedChunkPos); });
				}
			}
		}
	}

	const auto visibleChunks = chunkManager_.chunkStatusManager.GetVisibleChunks();
	for (auto visibleChunk : visibleChunks)
	{
		SetChunkOcclusionCulling(visibleChunk);
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
	std::lock_guard<std::mutex> lock(mutex_);
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
