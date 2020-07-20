#include "minelib/chunks/chunk_system.h"

#ifdef EASY_PROFILE_USE
#include <easy/profiler.h>
#endif
#include "engine/transform.h"

#include <minelib/chunks/chunk_manager.h>

#include "minelib/frustum.h"
#include "minelib/gizmos_renderer.h"
#include "minelib/minecraft_like_engine.h"
#include <PerlinNoise.hpp>
#include <minelib/map_generation.h>

namespace neko
{
ChunkSystem::ChunkSystem(MinecraftLikeEngine& engine)
	: blockManager_(engine.blockManager),
	  chunkManager_(engine.componentsManagerSystem.chunkManager),
	  entityManager_(engine.entityManager),
	  engine_(engine)
{
}

bool ChunkSystem::CalculateOcclusionStatus(
	const ChunkContentVector& chunkContent,
	ChunkFlag occludeDir) const
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("ChunkSystem::CalculateOcclusionStatus", profiler::colors::Blue900);
#endif
	//If chunk is full it occludes
	if (chunkContent.blocks.size() == kChunkBlockCount)
	{
		return true;
	}
	//If chunk is empty it didn't occludes
	if (chunkContent.blocks.size() == 0)
	{
		return false;
	}
	//Check if the side is full
	switch (static_cast<ChunkFlag>(occludeDir))
	{
		case ChunkFlag::OCCLUDE_DOWN:
			for (int x = 0; x < kChunkSize; x++)
			{
				for (int z = 0; z < kChunkSize; z++)
				{
					if (chunkContent.HasBlockAt(Vec3i(x, 0, z)))
					{
						return false;
					}
				}
			}
			break;
		case ChunkFlag::OCCLUDE_UP:
			for (int x = 0; x < kChunkSize; x++)
			{
				for (int z = 0; z < kChunkSize; z++)
				{
					if (chunkContent.HasBlockAt(Vec3i(x, kChunkSize - 1, z)))
					{
						return false;
					}
				}
			}
			break;
		case ChunkFlag::OCCLUDE_RIGHT:
			for (int x = 0; x < kChunkSize; x++)
			{
				for (int y = 0; y < kChunkSize; y++)
				{
					if (chunkContent.HasBlockAt(Vec3i(x, y, kChunkSize - 1)))
					{
						return false;
					}
				}
			}
			break;
		case ChunkFlag::OCCLUDE_LEFT:
			for (int x = 0; x < kChunkSize; x++)
			{
				for (int y = 0; y < kChunkSize; y++)
				{
					if (chunkContent.HasBlockAt(Vec3i(x, y, 0)))
					{
						return false;
					}
				}
			}
			break;
		case ChunkFlag::OCCLUDE_FRONT:
			for (int z = 0; z < kChunkSize; z++)
			{
				for (int y = 0; y < kChunkSize; y++)
				{
					if (chunkContent.HasBlockAt(Vec3i(0, y, z)))
					{
						return false;
					}
				}
			}
			break;
		case ChunkFlag::OCCLUDE_BACK:
			for (int z = 0; z < kChunkSize; z++)
			{
				for (int y = 0; y < kChunkSize; y++)
				{
					if (chunkContent.HasBlockAt(Vec3i(kChunkSize - 1, y, z)))
					{
						return false;
					}
				}
			}
			break;
		default: ;
	}
	return true;
}

Entity ChunkSystem::GenerateChunkContent(Entity newChunkIndex, const Vec3i& pos)
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("ChunkSystem::GenerateChunk", profiler::colors::Blue);
#endif
	ChunkContentVector chunkContent;
	ChunkMask chunkMask = 0;
	const auto randBlock = blockManager_.GetRandomBlock();
	//const auto randBlock = blockManager_.GetBlock(RandomRange(1, 6));
	if (pos.y > kHeightChunkLimit / 2)
	{
	}
	else if (pos.y < kHeightChunkLimit / 2)
	{
		const auto stoneBlock = blockManager_.GetBlock(RandomRange(1, 6));
		chunkContent.FillOfBlock(stoneBlock);
		chunkMask |= ChunkMask(ChunkFlag::OCCLUDE_DOWN);
		chunkMask |= ChunkMask(ChunkFlag::OCCLUDE_BACK);
		chunkMask |= ChunkMask(ChunkFlag::OCCLUDE_FRONT);
		chunkMask |= ChunkMask(ChunkFlag::OCCLUDE_LEFT);
		chunkMask |= ChunkMask(ChunkFlag::OCCLUDE_RIGHT);
		chunkMask |= ChunkMask(ChunkFlag::OCCLUDE_UP);
	}
	else
	{
		std::array<std::array<std::array<int, kChunkSize>, kChunkSize>, kChunkSize> map =
			mapGeneration.GenerateMap3D(pos.x * kChunkSize, pos.z * kChunkSize);
		for (uint16_t x = 0; x < kChunkSize; x++)
		{
			for (uint16_t y = 0; y < kChunkSize; y++)
			{
				for (uint16_t z = 0; z < kChunkSize; z++)
				{
					switch (map[x][y][z])
					{
						case 0:
							break;
						case 1:
							chunkContent.SetBlock(randBlock,
							                      PosToBlockId(Vec3i(x, kChunkSize - y, z)));
							break;
						case 2:
							chunkContent.SetBlock(randBlock,
							                      PosToBlockId(Vec3i(x, kChunkSize - y, z)));
							break;
						case 3:
							chunkContent.SetBlock(randBlock,
							                      PosToBlockId(Vec3i(x, kChunkSize - y, z)));
							break;
						case 4:
							chunkContent.SetBlock(randBlock,
							                      PosToBlockId(Vec3i(x, kChunkSize - y, z)));
							break;
						case 5:
							chunkContent.SetBlock(randBlock,
							                      PosToBlockId(Vec3i(x, kChunkSize - y, z)));
							break;
					}
				}
			}
		}
		chunkMask |= ChunkMask(ChunkFlag::OCCLUDE_DOWN);
	}
	//TODO(@Luca) : Implement CalculateOcclusionStatus
	//for (std::uint16_t occlude = static_cast<std::uint16_t>(ChunkFlag::OCCLUDE_DOWN); occlude <=
	//	static_cast<std::uint16_t>(ChunkFlag::OCCLUDE_BACK); occlude = occlude << 1u)
	//{
	//	if (CalculateOcclusionStatus(chunkContent, static_cast<ChunkFlag>(occlude)))
	//	{
	//		chunkMask |= ChunkMask(occlude);
	//	}
	//}

	//Set if the chunk is empty or not 
	if (chunkContent.blocks.size() == 0)
		chunkManager_.chunkStatusManager.AddStatus(newChunkIndex, ChunkFlag::EMPTY);
	else
		chunkManager_.chunkStatusManager.RemoveStatus(newChunkIndex, ChunkFlag::EMPTY);

	//Add occlusion status
	chunkManager_.chunkStatusManager.AddStatus(newChunkIndex, chunkMask);
	//Set chunkContent into component
	chunkManager_.chunkContentManager.FillOfBlocks(newChunkIndex, chunkContent);

	//chunkContent.CalculateBlockOcclusion();
	//Scheduled setting of render values
	std::lock_guard<std::mutex> lock(mutexRenderer_);
	scheduledRenderValues_.emplace_back([this, newChunkIndex, chunkContent]
	{
		chunkManager_.chunkRenderManager.Init(newChunkIndex);
		chunkManager_.chunkRenderManager.SetChunkValues(newChunkIndex, chunkContent);
	});

	return newChunkIndex;
}

void ChunkSystem::Init()
{
	std::lock_guard<std::mutex> lock(mutexRenderer_);
	scheduledRenderValues_.reserve(16);
	dirtyChunks_.reserve(kHeightChunkLimit * kChunkMaxViewDist);
	RendererLocator::get().Render(this);

	//Generate Map

	std::array<std::array<int, mapSize>, mapSize> heightMap;
	mapGeneration = MapGeneration();
	mapGeneration.GenerateZoneSurface(Vec2i(0, 0),
	                                  1,
	                                  1,
	                                  Zone(0, -1, 0, 0, Vec2i(0, 0), Vec2i(mapSize, mapSize)));
}

void ChunkSystem::CalculateVisibleStatus(const Entity chunkIndex) const
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("Chunks_System::CalculateVisibleStatus", profiler::colors::Pink100);
#endif
	const Vec3i currentChunkPos = chunkManager_.chunkPosManager.GetComponent(chunkIndex);
	bool visible = false;
	for (int chunk = 0; chunk < entityManager_.GetEntitiesSize(); chunk++)
	{
		if (!entityManager_.HasComponent(chunk,
		                                 static_cast<EntityMask>(ComponentType::CHUNK_POS)))
			continue;
		if (!chunkManager_.chunkStatusManager.HasStatus(chunk, ChunkFlag::LOADED))
			continue;
		const Vec3i chunkPos = chunkManager_.chunkPosManager.GetComponent(chunk);
		//Check if all sides are occlude
		for (std::uint16_t occlude = static_cast<std::uint16_t>(ChunkFlag::OCCLUDE_DOWN); occlude <=
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
			if (chunkPos == currentChunkPos + offset)
			{
				if (!chunkManager_.chunkStatusManager.HasStatus(chunk, ChunkFlag::OCCLUDED) && !
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
	//Check if all sides are occlude, set chunk has occluded
	if (!visible)
	{
		chunkManager_.chunkStatusManager.AddStatus(chunkIndex, ChunkFlag::OCCLUDED);
	}
}

void ChunkSystem::UpdateVisibleChunks()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("Chunks_System::UpdateVisibleChunks", profiler::colors::Green);
#endif

	auto chunks = chunkManager_.chunkStatusManager.GetLoadedChunks();
	//TODO (@Luca) : Find another way to get Camera
	const Vec3f viewerPos = GizmosLocator::get().GetCameraPos();
	const Vec3i currentChunkPos = Vec3i(std::floor(viewerPos.x / kChunkSize),
	                                    0,
	                                    std::floor(viewerPos.z / kChunkSize));
	Frustum frustum(*GizmosLocator::get().GetCamera());
	//Remove last visible chunks and accessible chunks
	for (auto chunk : chunks)
	{
		const Vec3i chunkPos = chunkManager_.chunkPosManager.GetComponent(chunk);
		if ((chunkPos.x > currentChunkPos.x + kChunkMaxViewDist ||
			chunkPos.z > currentChunkPos.z + kChunkMaxViewDist ||
			chunkPos.x < currentChunkPos.x - kChunkMaxViewDist ||
			chunkPos.z < currentChunkPos.z - kChunkMaxViewDist))
		{
			chunkManager_.chunkStatusManager.RemoveStatus(chunk, ChunkFlag::VISIBLE);
		}
		if ((chunkPos.x > currentChunkPos.x + kChunkAccessibleDist ||
			chunkPos.z > currentChunkPos.z + kChunkAccessibleDist ||
			chunkPos.x < currentChunkPos.x - kChunkAccessibleDist ||
			chunkPos.z < currentChunkPos.z - kChunkAccessibleDist))
		{
			chunkManager_.chunkStatusManager.RemoveStatus(chunk, ChunkFlag::ACCESSIBLE);
		}
		if (chunkManager_.chunkStatusManager.HasStatus(chunk, ChunkFlag::DIRTY))
		{
			const ChunkContentVector chunkContentVector = chunkManager_.chunkContentManager.GetComponent(chunk);
			//ChunkMask chunkMask;
			//for (std::uint16_t occlude = static_cast<std::uint16_t>(ChunkFlag::OCCLUDE_DOWN);
			//	occlude <=
			//	static_cast<std::uint16_t>(ChunkFlag::OCCLUDE_BACK); occlude = occlude << 1u)
			//{
			//	if (CalculateOcclusionStatus(chunkContentVector, static_cast<ChunkFlag>(occlude)))
			//	{
			//		chunkMask |= ChunkMask(occlude);
			//	}
			//}
			//chunkManager_.chunkStatusManager.AddStatus(chunk, chunkMask);
			std::lock_guard<std::mutex> lock(mutexRenderer_);
			scheduledRenderValues_.emplace_back([this, chunk, chunkContentVector]
			{
				chunkManager_.chunkRenderManager.Init(chunk);
				chunkManager_.chunkRenderManager.SetChunkValues(chunk, chunkContentVector);
			});
			
		}
	}
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("Chunks_System::CheckVisiblesChunks", profiler::colors::Green100);
#endif
	//For each chunk in view dist
	for (int xOffset = -kChunkMaxViewDist; xOffset <= kChunkMaxViewDist; xOffset++)
	{
		for (int zOffset = -kChunkMaxViewDist; zOffset <= kChunkMaxViewDist; zOffset++)
		{
			for (int yOffset = 0; yOffset < kHeightChunkLimit; yOffset++)
			{
				Vec3i viewedChunkPos = currentChunkPos + Vec3i(xOffset, yOffset, zOffset);

#ifdef EASY_PROFILE_USE
				EASY_BLOCK("Chunks_System::find_if", profiler::colors::Green100);
#endif
				const auto it = std::find_if(chunks.begin(),
				                             chunks.end(),
				                             [this, viewedChunkPos](const Entity& chunk)
				                             {
					                             const Vec3i chunkPos = chunkManager_
					                                                    .chunkPosManager.
					                                                    GetComponent(chunk);
					                             return (chunkPos == viewedChunkPos);
				                             });
				//If chunk has not been generated, generate one
				if (it == chunks.end())
				{
#ifdef EASY_PROFILE_USE
					EASY_BLOCK("Chunks_System::CreateEntity", profiler::colors::Green100);
#endif
					const Entity newChunkIndex = entityManager_.CreateEntity();
					chunkManager_.AddComponent(newChunkIndex);
					chunkManager_.chunkPosManager.SetComponent(newChunkIndex, viewedChunkPos);
					if (Abs(xOffset) < kChunkAccessibleDist && Abs(zOffset) < kChunkAccessibleDist)
					{
						chunkManager_.chunkStatusManager.AddStatus(
							newChunkIndex,
							ChunkFlag::ACCESSIBLE);
					}
					std::lock_guard<std::mutex> lock(mutexGeneration_);
					scheduledGenerationJobs_.push_back(std::make_unique<Job>(
						[this, newChunkIndex, viewedChunkPos]
						{
							GenerateChunkContent(newChunkIndex, viewedChunkPos);
						}));
					engine_.ScheduleJob(scheduledGenerationJobs_.back().get(),
					                    JobThreadType::OTHER_THREAD);
					dirtyChunks_.push_back(newChunkIndex);
				}
					//If chunk has already been generated
				else
				{
#ifdef EASY_PROFILE_USE
					EASY_BLOCK("Chunks_System::AddStatusVisible", profiler::colors::Green100);
#endif
					int index = std::distance(chunks.begin(), it);
					//Check if chunk is in the frustum
					if (frustum.Contains(chunkManager_.chunkPosManager.GetAabb(chunks[index])))
					{
						chunkManager_.chunkStatusManager.AddStatus(
							chunks[index],
							ChunkFlag::VISIBLE);
						if (Abs(xOffset) < kChunkAccessibleDist && Abs(zOffset) <
							kChunkAccessibleDist)
						{
							chunkManager_.chunkStatusManager.AddStatus(
								chunks[index],
								ChunkFlag::ACCESSIBLE);
						}
					}
					chunks.erase(chunks.begin() + index);
				}
			}
		}
	}

	bool allFinished = true;
	std::lock_guard<std::mutex> lock(mutexGeneration_);
	for (int i = 0; i < scheduledGenerationJobs_.size(); i++)
	{
		if (!scheduledGenerationJobs_[i]->IsDone())
		{
			allFinished = false;
			break;
		}
	}
	if (allFinished)
	{
		scheduledGenerationJobs_.clear();
		//Calculate if all new chunk are visible
		for (auto dirtyChunk : dirtyChunks_)
		{
			if (!chunkManager_.chunkStatusManager.HasStatus(dirtyChunk, ChunkFlag::EMPTY))
			{
#ifdef EASY_PROFILE_USE
				EASY_BLOCK("Chunks_System::scheduledGenerationJobs_", profiler::colors::Green100);
#endif
				scheduledGenerationJobs_.push_back(std::make_unique<Job>(
					[this, dirtyChunk]
					{
						CalculateVisibleStatus(dirtyChunk);
					}));
				engine_.ScheduleJob(scheduledGenerationJobs_.back().get(),
				                    JobThreadType::OTHER_THREAD);
			}
		}
		dirtyChunks_.clear();
	}
}

void ChunkSystem::Update(seconds dt)
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("ChunkSystem::Update", profiler::colors::Green);
#endif
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
			if (chunkManager_.chunkStatusManager.HasStatus(loadedChunk, ChunkFlag::VISIBLE))
			{
				if (chunkManager_.chunkStatusManager.HasStatus(loadedChunk, ChunkFlag::ACCESSIBLE))
				{
					GizmosLocator::get().DrawCube(
						Vec3f(chunkManager_.chunkPosManager.GetComponent(loadedChunk) * kChunkSize)
						+ Vec3f(
							(kChunkSize - 1) / 2.0f),
						Vec3f::one * kChunkSize,
						Color::green);
				}
				else
				{
					GizmosLocator::get().DrawCube(
						Vec3f(chunkManager_.chunkPosManager.GetComponent(loadedChunk) * kChunkSize)
						+ Vec3f(
							(kChunkSize - 1) / 2.0f),
						Vec3f::one * kChunkSize,
						Color::red);
				}
			}
			else
			{
				GizmosLocator::get().DrawCube(
					Vec3f(chunkManager_.chunkPosManager.GetComponent(loadedChunk) * kChunkSize) +
					Vec3f(
						(kChunkSize - 1) / 2.0f),
					Vec3f::one * kChunkSize,
					Color::blue);
			}
		}
	}
}

void ChunkSystem::Render()
{
	std::lock_guard<std::mutex> lock(mutexRenderer_);
	while (!scheduledRenderValues_.empty())
	{
		auto currentTask = scheduledRenderValues_.front();
		if (!currentTask) continue;

		currentTask();
		scheduledRenderValues_.erase(scheduledRenderValues_.begin());
	}
}

void ChunkSystem::Destroy()
{
}

void ChunkSystem::DrawImGui()
{
	ImGui::Begin("Chunk Generation");
	float count = 0;
	std::lock_guard<std::mutex> lock(mutexGeneration_);
	for (int i = 0; i < scheduledGenerationJobs_.size(); i++)
	{
		if (scheduledGenerationJobs_[i]->IsDone())
		{
			count++;
		}
	}
	if (scheduledGenerationJobs_.size() > 0)
	{
		int loading = (count / scheduledGenerationJobs_.size()) * 100;
		ImGui::Text("Loading : %i / 100", loading);
	}
	ImGui::End();
}
}
