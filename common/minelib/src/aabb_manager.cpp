#include "minelib/aabb_manager.h"

#include "minelib/minecraft_like_engine.h"

#include "engine/transform.h"

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
	for (size_t i = 0; i < INIT_ENTITY_NMB; i++)
	{
		if (chunksManager_.GetComponent(i).GetAabb().IntersectRay(origin, dir))
		{
			return i;
		}
	}
	return INVALID_INDEX;
}

AabbBlock AabbManager::RaycastBlock(Vec3f origin, Vec3f dir)
{
	float minDist = std::numeric_limits<float>::max();
	Vec3i blockPos = Vec3i(kInvalidPos_);
	Vec3f blockChunkPos = Vec3f(static_cast<float>(kInvalidPos_));
	AabbBlock block;
	for (size_t c = 0; c < INIT_ENTITY_NMB; c++)
	{
		if (!engine_.entityManager_.HasComponent(c, static_cast<EntityMask>(ComponentType::CHUNK))) { continue; }
		Chunk chunk = engine_.componentsManagerSystem_.chunkManager_.GetComponent(c);
		Vec3f chunkPos = engine_.componentsManagerSystem_.transform3dManager_.GetPosition(c);

		for (size_t i = 0; i < kChunkSize * kChunkSize * kChunkSize; i++)
		{
			int z = std::floor(i / (kChunkSize * kChunkSize));
			int y = std::floor((i - z * kChunkSize * kChunkSize) / kChunkSize);
			int x = i % kChunkSize;
			int blockID = chunk.GetBlockId(Vec3i(x, y, z));
			if (blockID == 0) continue;
			Aabb3d aabb;
			aabb.SetFromCenter(Vec3f(x, y, z) + chunkPos, Vec3f::one / 2);
			float rayDist = aabb.IntersectRay(dir, origin, minDist);
			if (rayDist > 0 && rayDist < minDist)
			{
				minDist = rayDist;
				blockPos = Vec3i(x, y, z);
				blockChunkPos = chunkPos;
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

AabbBlock AabbManager::RaycastBlockInChunk(Vec3f origin, Vec3f dir, Index chunkIndex)
{
	if (!engine_.entityManager_.HasComponent(chunkIndex, static_cast<EntityMask>(ComponentType::CHUNK))) { return AabbBlock(); }
	Chunk chunk = engine_.componentsManagerSystem_.chunkManager_.GetComponent(chunkIndex);
	Vec3f chunkPos = engine_.componentsManagerSystem_.transform3dManager_.GetPosition(chunkIndex);

	AabbBlock block;
	float minDist = std::numeric_limits<float>::max();
	Vec3i blockPos = Vec3i(-1);
	for (size_t i = 0; i < kChunkSize * kChunkSize * kChunkSize; i++)
	{
		int z = std::floor(i / (kChunkSize * kChunkSize));
		int y = std::floor((i - z * kChunkSize * kChunkSize) / kChunkSize);
		int x = i % kChunkSize;
		int blockID = chunk.GetBlockId(Vec3i(x, y, z));
		if (blockID == 0) continue;
		Aabb3d aabb;
		aabb.SetFromCenter(Vec3f(x, y, z), Vec3f::one / 2);
		float rayDist = aabb.IntersectRay(dir, origin, minDist);
		if (rayDist > 0 && rayDist < minDist)
		{
			minDist = rayDist;
			blockPos = Vec3i(x, y, z);
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
