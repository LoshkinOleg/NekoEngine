#include "aabb_manager.h"

#include "engine/transform.h"

#include "minecraft_like_engine.h"

namespace neko
{
AabbManager::AabbManager(MinecraftLikeEngine& engine)
	: engine_(engine),
	  chunksManager_(engine.componentsManagerSystem_.chunkManager_),
	  transform3dManager_(engine.componentsManagerSystem_.transform3dManager_),
	  entityManager_(engine.entityManager_)
{
	AabbLocator::provide(this);
}

Index AabbManager::RaycastChunk(Vec3f origin, Vec3f dir)
{
	const auto visibleChunks = chunksManager_.GetVisibleChunks();
	for (auto visibleChunk : visibleChunks)
	{
		if (chunksManager_.GetComponent(visibleChunk).GetAabb().IntersectRay(origin, dir))
		{
			return visibleChunk;
		}
	}
	return INVALID_INDEX;
}

Block AabbManager::RaycastBlock(Vec3f origin, Vec3f dir)
{
	float minDist = std::numeric_limits<float>::max();
	Vec3i blockPos = Vec3i(kInvalidPos_);
	Vec3f blockChunkPos = Vec3f(static_cast<float>(kInvalidPos_));
	Block block;
	const auto visibleChunks = chunksManager_.GetVisibleChunks();
	for (auto visibleChunk : visibleChunks)
	{
		if (!engine_.entityManager_.HasComponent(visibleChunk, static_cast<EntityMask>(ComponentType::CHUNK))) { continue; }
		Chunk chunk = engine_.componentsManagerSystem_.chunkManager_.GetComponent(visibleChunk);
		Vec3f chunkPos = engine_.componentsManagerSystem_.transform3dManager_.GetPosition(visibleChunk);

		for (size_t x = 0; x < kChunkSize; x++)
		{
			for (size_t y = 0; y < kChunkSize; y++)
			{
				for (size_t z = 0; z < kChunkSize; z++)
				{
					int blockID = chunk.GetBlockId(Vec3i(x, y, z));
					if (blockID == 0) continue;
					Aabb3d aabb;
					aabb.SetFromCenter(Vec3f(x, y, z) + chunkPos, Vec3f::one / 2);
					float rayDist = aabb.CalculateRayDist(dir, origin, minDist);
					if (rayDist > 0 && rayDist < minDist)
					{
						minDist = rayDist;
						blockPos = Vec3i(x, y, z);
						blockChunkPos = chunkPos;
					}
				}
			}
		}
		if (blockPos != Vec3i(-1))
		{
			block.blockPos = Vec3f(blockPos.x, blockPos.y, blockPos.z) + blockChunkPos;
			block.blockType = chunk.GetBlockId(blockPos);
		}
	}
	return block;
}

Block AabbManager::RaycastBlockInChunk(Vec3f origin, Vec3f dir, Index chunkIndex)
{
	if (!engine_.entityManager_.HasComponent(chunkIndex, static_cast<EntityMask>(ComponentType::CHUNK))) { return Block(); }
	Chunk chunk = engine_.componentsManagerSystem_.chunkManager_.GetComponent(chunkIndex);
	Vec3f chunkPos = engine_.componentsManagerSystem_.transform3dManager_.GetPosition(chunkIndex);

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
