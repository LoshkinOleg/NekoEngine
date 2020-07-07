#include "minelib/aabb_manager.h"

#include "minelib/minecraft_like_engine.h"

#include "engine/transform.h"

#include <minelib/chunks/chunk_manager.h>

namespace neko
{
AabbManager::AabbManager(MinecraftLikeEngine& engine)
	: engine_(engine),
	  entityManager_(engine.entityManager),
	  chunkPosManager_(engine.componentsManagerSystem.chunkPosManager),
	  chunkContentManager_(engine.componentsManagerSystem.chunkContentManager),
	  transform3dManager_(engine.componentsManagerSystem.transform3dManager)
{
	AabbLocator::provide(this);
}

std::vector<Index> AabbManager::RaycastChunk(const Vec3f& origin, const Vec3f& dir)
{
	std::vector<Index> raycastedChunks;
	const auto chunksNmb = entityManager_.GetEntitiesNmb(static_cast<EntityMask>(ComponentType::CHUNK_POS));
	for (Index chunkIndex = 0; chunkIndex < chunksNmb; chunkIndex++)
	{
		if (chunkPosManager_.GetAabb(chunksNmb).IntersectRay(origin, dir))
		{
			raycastedChunks.push_back(chunkIndex);
		}
	}
	return raycastedChunks;
}

AabbBlock AabbManager::RaycastBlock(const Vec3f& origin, const Vec3f& dir)
{
	float minDist = std::numeric_limits<float>::max();
	Vec3i blockPos = Vec3i(kInvalidPos_);
	Vec3f blockChunkPos = Vec3f(static_cast<float>(kInvalidPos_));
	AabbBlock block;
	const auto chunksNmb = entityManager_.GetEntitiesNmb(static_cast<EntityMask>(ComponentType::CHUNK_POS));
	for (Index chunkIndex = 0; chunkIndex < chunksNmb; chunkIndex++)
	{
		const Vec3f chunkPos = transform3dManager_.GetPosition(chunkIndex);
		bool intersect = false;
		for (unsigned x = 0; x < kChunkSize; x++)
		{
			for (unsigned y = 0; y < kChunkSize; y++)
			{
				for (unsigned z = 0; z < kChunkSize; z++)
				{
					const int blockId = chunkContentManager_.GetBlockId(chunkIndex, Vec3i(x, y, z));
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
			block.blockType = chunkContentManager_.GetBlockId(chunkIndex, blockPos);
		}
	}
	return block;
}

AabbBlock AabbManager::RaycastBlockInChunk(const Vec3f& origin, const Vec3f& dir, const Index chunkIndex)
{
	if (!entityManager_.HasComponent(chunkIndex, static_cast<EntityMask>(ComponentType::CHUNK_POS))) { return AabbBlock(); }
	const Vec3f chunkPos = transform3dManager_.GetPosition(chunkIndex);

	AabbBlock block;
	float minDist = std::numeric_limits<float>::max();
	Vec3i blockPos = Vec3i(-1);
	for (size_t x = 0; x < kChunkSize; x++)
	{
		for (size_t y = 0; y < kChunkSize; y++)
		{
			for (size_t z = 0; z < kChunkSize; z++)
			{
				int blockID = chunkContentManager_.GetBlockId(chunkIndex, Vec3i(x, y, z));
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
		block.blockType = chunkContentManager_.GetBlockId(chunkIndex, blockPos);
	}
	return block;
}
}
