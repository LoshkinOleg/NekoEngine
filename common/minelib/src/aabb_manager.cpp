#include "minelib/aabb_manager.h"

#include "minelib/minecraft_like_engine.h"

#include "engine/transform.h"
#include "minelib/blocks/block.h"

#include <minelib/chunks/chunk_manager.h>

namespace neko
{
AabbManager::AabbManager(MinecraftLikeEngine& engine)
	: engine_(engine),
	  entityManager_(engine.entityManager),
	  chunkManager_(engine.componentsManagerSystem.chunkManager),
	  transform3dManager_(engine.componentsManagerSystem.transform3dManager)
{
	AabbLocator::provide(this);
}

bool AabbManager::RaycastChunk(Ray& ray,
                               const Vec3f& origin,
                               const Vec3f& dir) const
{
	float rayDist;
	const auto visibleChunks = chunkManager_.chunkStatusManager.GetVisibleChunks();
	for (auto visibleChunk : visibleChunks)
	{
		const Aabb3d aabb = chunkManager_.chunkPosManager.GetAabb(visibleChunk);
		if (aabb.IntersectRay(origin, dir, rayDist))
		{
			ray.hitChunk = visibleChunk;
			ray.hitDist = rayDist;
			ray.hitAabb = aabb;
			return true;
		}
	}
	return false;
}

std::vector<Index> AabbManager::RaycastChunks(const Vec3f& origin, const Vec3f& dir) const
{
	float rayDist;
	std::vector<Index> indices;
	const auto visibleChunks = chunkManager_.chunkStatusManager.GetVisibleChunks();
	for (auto visibleChunk : visibleChunks)
	{
		const Aabb3d aabb = chunkManager_.chunkPosManager.GetAabb(visibleChunk);
		if (aabb.IntersectRay(origin, dir, rayDist))
		{
			indices.push_back(visibleChunk);
		}
	}
	return indices;
}

bool AabbManager::RaycastBlock(Ray& ray,
                               const Vec3f& origin,
                               const Vec3f& dir) const
{
	float rayDist;
	const auto visibleChunks = chunkManager_.chunkStatusManager.GetVisibleChunks();
	for (auto visibleChunk : visibleChunks)
	{
		const Vec3f chunkPos = Vec3f(chunkManager_.chunkPosManager.GetComponent(visibleChunk));

		const auto chunkContent = chunkManager_.chunkContentManager.GetBlocks(visibleChunk);
		for (auto& block : chunkContent)
		{
			const Vec3f blockPos = Vec3f(BlockIdToPos(block.blockId)) + chunkPos * kChunkSize;
			
			Aabb3d aabb;
			aabb.SetFromCenter(blockPos, Vec3f(kCubeHalfSize));
			if (aabb.IntersectRay(dir, origin, rayDist) && rayDist > 0 && rayDist < ray.hitDist)
			{
				ray.hitId = block.blockId;
				ray.hitChunk = visibleChunk;
				ray.hitDist = rayDist;
				ray.hitAabb = aabb;
			}
		}
	}
	return ray.hitId != INVALID_INDEX;
}

bool AabbManager::RaycastBlockInChunk(Ray& ray,
                                      const Vec3f& origin,
                                      const Vec3f& dir,
                                      const Index chunkIndex) const
{
	if (!entityManager_.HasComponent(chunkIndex, static_cast<EntityMask>(ComponentType::CHUNK_POS))) { return false; }
	const Vec3f chunkPos = Vec3f(chunkManager_.chunkPosManager.GetComponent(chunkIndex));

	float rayDist;
	const auto chunkContent = chunkManager_.chunkContentManager.GetBlocks(chunkIndex);
	for (auto& block : chunkContent)
	{
		const Vec3f blockPos = Vec3f(BlockIdToPos(block.blockId)) + chunkPos * kChunkSize;
		
		Aabb3d aabb;
		aabb.SetFromCenter(blockPos, Vec3f(kCubeHalfSize));
		if (aabb.IntersectRay(dir, origin, rayDist) && rayDist > 0 && rayDist < ray.hitDist)
		{
			ray.hitId = block.blockId;
			ray.hitChunk = chunkIndex;
			ray.hitDist = rayDist;
			ray.hitAabb = aabb;
		}
	}
	return ray.hitId != INVALID_INDEX;
}
}
