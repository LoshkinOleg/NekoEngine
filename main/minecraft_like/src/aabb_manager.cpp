#include "aabb_manager.h"

#include "minecraft_like_engine.h"

namespace neko
{
AabbManager::AabbManager(MinecraftLikeEngine& engine)
	: engine_(engine),
	  chunksManager_(engine.componentsManagerSystem_.chunkManager_),
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

Block AabbManager::RaycastBlock(Vec3f origin, Vec3f dir)
{
	float minDist = std::numeric_limits<float>::max();
	Vec3i blockPos = Vec3i(-1);
	Block block;
	for (size_t c = 0; c < INIT_ENTITY_NMB; c++)
	{
		if (!engine_.entityManager_.HasComponent(c, static_cast<EntityMask>(ComponentType::CHUNK))) { continue; }
		Chunk chunk = engine_.componentsManagerSystem_.chunkManager_.GetComponent(c);
		Vec3f chunkPos = engine_.componentsManagerSystem_.transform3dManager_.GetPosition(c);

		for (size_t i = 0; i < chunkSize * chunkSize * chunkSize; i++)
		{
			int z = std::floor(i / (chunkSize * chunkSize));
			int y = std::floor((i - z * chunkSize * chunkSize) / chunkSize);
			int x = i % chunkSize;
			int blockID = chunk.GetBlockId(Vec3i(x, y, z));
			if (blockID == 0) continue;
			Aabb3d aabb;
			aabb.SetFromCenter(Vec3f(x, y, z) + chunkPos, Vec3f::one / 2);
			float rayDist = aabb.CalculateRayDist(dir, origin, minDist);
			if (rayDist > 0 && rayDist < minDist)
			{
				minDist = rayDist;
				blockPos = Vec3i(x, y, z);
			}
		}
		if (blockPos != Vec3i(-1))
		{
			block.chunkIndex = c;
			block.blockPos = blockPos;
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
	for (size_t i = 0; i < chunkSize * chunkSize * chunkSize; i++)
	{
		int z = std::floor(i / (chunkSize * chunkSize));
		int y = std::floor((i - z * chunkSize * chunkSize) / chunkSize);
		int x = i % chunkSize;
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
	if (blockPos != Vec3i(-1))
	{
		block.chunkIndex = chunkIndex;
		block.blockPos = blockPos;
		block.blockType = chunk.GetBlockId(blockPos);
	}
	return block;
}
}
