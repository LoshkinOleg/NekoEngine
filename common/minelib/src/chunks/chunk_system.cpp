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
	  entityManager_(engine.entityManager)
{
}

Entity ChunkSystem::GenerateChunkArray(const Vec3i& pos) const
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("ChunkSystem::GenerateChunk", profiler::colors::Blue);
#endif
	const Entity newChunkIndex = entityManager_.CreateEntity();
	chunkManager_.AddComponent(newChunkIndex);

	//const auto randBlock = blockManager_.GetRandomBlock();
	const auto randBlock = blockManager_.GetBlock(RandomRange(1, 6));
	chunkManager_.chunkPosManager.SetComponent(newChunkIndex, pos);
	if (pos.y > kHeightChunkLimit / 2)
	{
	}
	else if (pos.y < kHeightChunkLimit / 2)
	{
		const auto stoneBlock = blockManager_.GetBlock(RandomRange(1, 6));
		chunkManager_.chunkContentManager.FillOfBlock(newChunkIndex, stoneBlock);
		chunkManager_.chunkStatusManager.AddStatus(newChunkIndex, ChunkFlag::OCCLUDE_DOWN);
		chunkManager_.chunkStatusManager.AddStatus(newChunkIndex, ChunkFlag::OCCLUDE_UP);
		chunkManager_.chunkStatusManager.AddStatus(newChunkIndex, ChunkFlag::OCCLUDE_LEFT);
		chunkManager_.chunkStatusManager.AddStatus(newChunkIndex, ChunkFlag::OCCLUDE_RIGHT);
		chunkManager_.chunkStatusManager.AddStatus(newChunkIndex, ChunkFlag::OCCLUDE_FRONT);
		chunkManager_.chunkStatusManager.AddStatus(newChunkIndex, ChunkFlag::OCCLUDE_BACK);
	}
	else
	{
		for (uint16_t x = 0; x < kChunkSize; x++)
		{
			for (uint16_t z = 0; z < kChunkSize; z++)
			{
				chunkManager_.chunkContentManager.SetBlock(newChunkIndex,
				                                           PosToBlockId(Vec3i(x, 0, z)),
				                                           randBlock);
			}
		}
		if (randBlock.name != "Glass")
		{
			chunkManager_.chunkStatusManager.AddStatus(newChunkIndex, ChunkFlag::OCCLUDE_DOWN);
		}
	}
#ifdef EASY_PROFILE_USE
	EASY_END_BLOCK
#endif
	return newChunkIndex;
}

void ChunkSystem::Init()
{
	std::lock_guard<std::mutex> lock(mutex_);
	scheduledChunks_.reserve(16);
	RendererLocator::get().Render(this);
}

void ChunkSystem::SetChunkOcclusionCulling(const Entity chunkIndex) const
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("Chunks_System::SetChunkOcclusionCulling", profiler::colors::Pink100);
#endif
	const Vec3i pos = chunkManager_.chunkPosManager.GetComponent(chunkIndex);
	bool visible = false;
	const auto chunks = entityManager_.FilterEntities(
		static_cast<EntityMask>(ComponentType::CHUNK_STATUS));
	for (auto chunk : chunks)
	{
		if (!entityManager_.HasComponent(chunk,
		                                 static_cast<EntityMask>(ComponentType::CHUNK_POS)))
			continue;

		const Vec3i chunkPos = chunkManager_.chunkPosManager.GetComponent(chunk);
		for (auto occlude = static_cast<std::uint16_t>(ChunkFlag::OCCLUDE_DOWN); occlude <=
		     static_cast<std::uint16_t>(ChunkFlag::OCCLUDE_BACK); occlude = occlude << 1u)
		{
			Vec3i offset;
			switch (static_cast<ChunkFlag>(occlude))
			{
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
				if (chunkManager_.chunkStatusManager.HasStatus(chunk, ChunkFlag::VISIBLE) && !
					chunkManager_.chunkStatusManager.HasStatus(
						chunk,
						static_cast<ChunkFlag>(occlude)))
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
		//chunkManager_.chunkStatusManager.RemoveStatus(chunkIndex, ChunkFlag::VISIBLE);
		chunkManager_.chunkStatusManager.AddStatus(chunkIndex, ChunkFlag::OCCLUDED);
	}
}

void ChunkSystem::UpdateVisibleChunks()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("Chunks_System::UpdateVisibleChunks", profiler::colors::Green);
#endif
	//Remove last visible chunks

	auto chunks = entityManager_.FilterEntities(
		static_cast<EntityMask>(ComponentType::CHUNK_CONTENT));
	const Vec3f viewerPos = GizmosLocator::get().GetCamera()->position;
	const Vec3i currentChunkPos = Vec3i(std::floor(viewerPos.x / kChunkSize),
	                                    0,
	                                    std::floor(viewerPos.z / kChunkSize));

	const int drawSize = kMaxViewDist / kChunkSize;
	std::vector<Entity> dirtyChunks;
	dirtyChunks.reserve(kHeightChunkLimit * drawSize);
	for (auto chunk : chunks)
	{
		const Vec3i chunkPos = chunkManager_.chunkPosManager.GetComponent(chunk);
		if (chunkPos.x > currentChunkPos.x + drawSize ||
			chunkPos.z > currentChunkPos.z + drawSize ||
			chunkPos.x < currentChunkPos.x - drawSize ||
			chunkPos.z < currentChunkPos.z - drawSize)
			chunkManager_.chunkStatusManager.RemoveStatus(chunk, ChunkFlag::VISIBLE);
	}
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("Chunks_System::CheckVisiblesChunks", profiler::colors::Green100);
#endif
	for (int xOffset = -drawSize; xOffset <= drawSize; xOffset++)
	{
		for (int zOffset = -drawSize; zOffset <= drawSize; zOffset++)
		{
			for (int yOffset = 0; yOffset < kHeightChunkLimit; yOffset++)
			{
				Vec3i viewedChunkPos = currentChunkPos + Vec3i(xOffset, yOffset, zOffset);

				const auto it = std::find_if(chunks.begin(), chunks.end(),
				[this, viewedChunkPos](const Entity& chunk)
				{
				    const Vec3i chunkPos = chunkManager_.chunkPosManager.GetComponent(chunk);
				    return chunkPos == viewedChunkPos;
				});
				
				std::lock_guard<std::mutex> lock(mutex_);
				if (it == chunks.end())
				{
					Entity newChunkIndex = GenerateChunkArray(viewedChunkPos);
					scheduledChunks_.emplace_back([this, newChunkIndex]
					{
						chunkManager_.chunkRenderManager.Init(newChunkIndex);
						chunkManager_.chunkRenderManager.SetChunkValues(newChunkIndex);
					});
					dirtyChunks.push_back(newChunkIndex);
				}
				else
				{
					const int index = std::distance(chunks.begin(), it);
					chunkManager_.chunkStatusManager.AddStatus(chunks[index], ChunkFlag::VISIBLE);
					chunks.erase(chunks.begin() + index);
				}
			}
		}
	}
#ifdef EASY_PROFILE_USE
	EASY_END_BLOCK
	EASY_BLOCK("Chunks_System::SetChunkOcclusionCulling", profiler::colors::Pink);
#endif
	for (auto dirtyChunk : dirtyChunks)
	{
		SetChunkOcclusionCulling(dirtyChunk);
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
				Vec3f(chunkManager_.chunkPosManager.GetComponent(loadedChunk) * kChunkSize) + Vec3f(
					(kChunkSize - 1) / 2.0f),
				Vec3f::one * kChunkSize,
				chunkManager_.chunkStatusManager.HasStatus(loadedChunk, ChunkFlag::VISIBLE)
					? Color::blue
					: Color::red);
		}
	}
}

void ChunkSystem::Render()
{
	std::lock_guard<std::mutex> lock(mutex_);
	while (!scheduledChunks_.empty())
	{
		auto currentTask = scheduledChunks_.front();
		if (!currentTask) continue;
		
		currentTask();
		scheduledChunks_.erase(scheduledChunks_.begin());
	}
}

void ChunkSystem::Destroy()
{
}
}
