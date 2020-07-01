#include "aabb_manager.h"

#include "engine/transform.h"

#include "minecraft_like_engine.h"

namespace neko
{
AabbManager::AabbManager(MinecraftLikeEngine& engine)
	: engine_(engine),
	  entityManager_(engine.entityManager_),
	  chunksManager_(engine.componentsManagerSystem_.chunksManager_),
	  transform3dManager_(engine.componentsManagerSystem_.transform3dManager_)
{
	AabbLocator::provide(this);
}

std::vector<Index> AabbManager::RaycastChunk(const Vec3f& origin, const Vec3f& dir)
{
	std::vector<Index> raycastedChunks;
	const auto visibleChunks = chunksManager_.GetVisibleChunks();
	for (auto visibleChunk : visibleChunks)
	{
		if (chunksManager_.GetComponent(visibleChunk).GetAabb().IntersectRay(origin, dir))
		{
			raycastedChunks.push_back(visibleChunk);
		}
	}
	return raycastedChunks;
}

Block AabbManager::RaycastBlock(const Vec3f& origin, const Vec3f& dir)
{
	float minDist = std::numeric_limits<float>::max();
	Vec3i blockPos = Vec3i(kInvalidPos_);
	Vec3f blockChunkPos = Vec3f(static_cast<float>(kInvalidPos_));
	Block block;
	const auto visibleChunks = chunksManager_.GetVisibleChunks();
	for (auto visibleChunk : visibleChunks)
	{
		if (!entityManager_.HasComponent(visibleChunk, static_cast<EntityMask>(ComponentType::CHUNK))) { continue; }
		const Chunk chunk = chunksManager_.GetComponent(visibleChunk);
		const Vec3f chunkPos = transform3dManager_.GetPosition(visibleChunk);
		bool intersect = false;
		for (unsigned x = 0; x < kChunkSize; x++)
		{
			for (unsigned y = 0; y < kChunkSize; y++)
			{
				for (unsigned z = 0; z < kChunkSize; z++)
				{
					const int blockId = chunk.GetBlockId(Vec3i(x, y, z));
					if (blockId == 0) continue;
					Aabb3d aabb;
					aabb.SetFromCenter(Vec3f(x, y, z) + chunkPos, Vec3f::one / 2);
					const float rayDist = aabb.CalculateRayDist(dir, origin, minDist);
					if (rayDist > 0 && rayDist < minDist)
					{
						minDist = rayDist;
						blockPos = Vec3i(x, y, z);
						blockChunkPos = chunkPos;
						intersect = true;
					}
				}
			}
		}
		if (intersect)
		{
			block.blockPos = Vec3f(blockPos.x, blockPos.y, blockPos.z) + blockChunkPos;
			block.blockType = chunk.GetBlockId(blockPos);
		}
	}
	return block;
}

Block AabbManager::RaycastBlockInChunk(const Vec3f& origin, const Vec3f& dir, const Index chunkIndex)
{
	if (!engine_.entityManager_.HasComponent(chunkIndex, static_cast<EntityMask>(ComponentType::CHUNK))) { return Block(); }
	const Chunk chunk = chunksManager_.GetComponent(chunkIndex);
	const Vec3f chunkPos = transform3dManager_.GetPosition(chunkIndex);

	Block block;
	float minDist = std::numeric_limits<float>::max();
	Vec3i blockPos = Vec3i(-1);
	for (size_t x = 0; x < kChunkSize; x++)
	{
		for (size_t y = 0; y < kChunkSize; y++)
		{
			for (size_t z = 0; z < kChunkSize; z++)
			{
				int blockID = chunk.GetBlockId(Vec3i(x, y, z));
				if (blockID == 0) continue;
				Aabb3d aabb;
				aabb.SetFromCenter(Vec3f(x, y, z), Vec3f::one / 2);
				float rayDist = aabb.CalculateRayDist(dir, origin, minDist);
				if (rayDist > 0 && rayDist < minDist)
				{
					minDist = rayDist;
					blockPos = Vec3i(x, y, z);
				}
			}
		}
	}
	if (blockPos != Vec3i(-1))
	{
		block.blockPos = Vec3f(blockPos.x, blockPos.y, blockPos.z) + chunkPos;
		block.blockType = chunk.GetBlockId(blockPos);
	}
	return block;
}
}
