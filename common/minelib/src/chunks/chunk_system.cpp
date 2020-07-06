#include "minelib/chunks/chunk_system.h"

#ifdef EASY_PROFILE_USE
#include <easy/profiler.h>
#endif
#include "engine/transform.h"

#include "minelib/chunks/chunk_manager.h"
#include "minelib/gizmos_renderer.h"
#include "minelib/minecraft_like_engine.h"
#include <PerlinNoise.hpp>

namespace neko
{
	ChunksSystem::ChunksSystem(MinecraftLikeEngine& engine)
		: chunksManager_(engine.componentsManagerSystem_.chunksManager_),
		transform3dManager_(engine.componentsManagerSystem_.transform3dManager_),
		entityManager_(engine.entityManager_)
{
}

Chunk ChunksSystem::GenerateChunk(const Vec3i& pos) const
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("ChunksSystem::GenerateChunk");
#endif
	Chunk chunk;
	const uint8_t randBlockId = RandomRange(1, 4);
	chunk.SetChunkPos(pos);
	std::array<std::array<int, kChunkSize>, kChunkSize> blocks = MapGeneration(offset, kChunkSize, kChunkSize, seed, frequency, octaves);
	for (unsigned x = 0; x < kChunkSize; x++)
	{
		for (unsigned y = 0; y < kChunkSize; y++)
		{
			chunk.SetBlock(randBlockId, Vec3i(x,blocks[x][y], y));			//TODO: Bug here, the perlin noise value is negative
		}
	}
	chunk.SetVisible(true);
	return chunk;

		/*Chunk chunk;
	const uint8_t randBlockId = RandomRange(1, 4);
	chunk.SetChunkPos(pos);
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
					chunk.SetBlock(randBlockId, Vec3i(x, y, z));
				}
			}
		}
	}
	chunk.SetVisible(true);
	return chunk;*/
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
	const auto visibleChunks = chunksManager_.GetVisibleChunks();
	for (auto visibleChunk : visibleChunks)
	{
		Chunk chunk = chunksManager_.GetComponent(visibleChunk);
		chunk.SetVisible(false);
		chunksManager_.SetComponent(visibleChunk, chunk);
	}
	chunksManager_.ClearVisibleChunks();
	chunksManager_.
		ReserveVisibleChunks((kMaxViewDist_ / kChunkSize) * (kMaxViewDist_ / kChunkSize));

	const Vec3f viewerPos = transform3dManager_.GetPosition(0);
	const Vec3i currentChunkPos = Vec3i(viewerPos.x / kChunkSize, 0, viewerPos.z / kChunkSize);

	for (int xOffset = -(kMaxViewDist_ / kChunkSize); xOffset <= (kMaxViewDist_ / kChunkSize);
	     xOffset++)
	{
		for (int zOffset = -(kMaxViewDist_ / kChunkSize); zOffset <= (kMaxViewDist_ / kChunkSize);
		     zOffset++)
		{
			Vec3i viewedChunkPos = currentChunkPos + Vec3i(xOffset, 0, zOffset);
			bool found = false;
			const auto loadedChunks = chunksManager_.GetLoadedChunks();
			for (auto loadedChunk : loadedChunks)
			{
				Chunk chunk = chunksManager_.GetComponent(loadedChunk);
				if (chunk.GetChunkPos() == viewedChunkPos)
				{
					chunk.SetVisible(true);
					chunksManager_.SetComponent(loadedChunk, chunk);
					chunksManager_.AddVisibleChunk(loadedChunk);
					found = true;
					break;
				}
			}
			if (!found)
			{
				const Entity newChunkIndex = entityManager_.CreateEntity();
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
	//Update Visible Chunks
	UpdateVisibleChunks();

	//Display Chunks Gizmos
	const Vec3f cubeOffset = Vec3f((kChunkSize - 1) / 2.0f);
	const auto loadedChunks = chunksManager_.GetLoadedChunks();
	for (auto loadedChunk : loadedChunks)
	{
		if (entityManager_.HasComponent(loadedChunk, static_cast<EntityMask>(ComponentType::CHUNK)))
		{
			GizmosLocator::get().DrawCube(
				transform3dManager_.GetPosition(loadedChunk) + Vec3f::one * kChunkSize/2 - Vec3f::one/2,
				Vec3f::one * kChunkSize,
				chunksManager_.GetComponent(loadedChunk).IsVisible()
					? Color4(1.0f, 0.0f, 0.0f, 1.0f)
					: Color4(0.0f, 0.0f, 1.0f, 1.0f));
		}
	}
}

std::array<std::array<int, 16>, 16> ChunksSystem::MapGeneration(Vec2<int> offset, int chunkSize, int chunkHeight, int seed, float frequency, int octaves)
{
	if (octaves <= 0)
	{
		octaves = 1;
	}
		
	//Map Generation
	std::array<std::array<int, 16>, 16> map;
	siv::PerlinNoise noise = siv::PerlinNoise(seed);
		
	for (int y = 0; y < chunkSize; y++)
	{
		for (int x = 0; x < chunkSize; x++)
		{
			float result = 0;
			for (int i = 0; i < octaves; i++)
			{
				float nx = (float)(x + offset.x) / (float)chunkSize - 0.5f, ny = (float)(y + offset.y) / (float)chunkSize - 0.5f;
				nx *= frequency;
				ny *= frequency;
				result = ((float)1 / (float)(i + 1) * (noise.noise(nx,ny)));
			}

			map[x][y] = (unsigned int)(result * chunkHeight);
		}
	}

	return map;
}


void ChunksSystem::Destroy()
{
}
}
