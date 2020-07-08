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
	  chunkPosManager_(engine.componentsManagerSystem.chunkPosManager),
	  chunkContentManager_(engine.componentsManagerSystem.chunkContentManager),
	  chunkStatutManager_(engine.componentsManagerSystem.chunkStatutManager),
	  transform3dManager_(engine.componentsManagerSystem.transform3dManager)
{
	AabbLocator::provide(this);
}

bool AabbManager::RaycastChunk(Ray& ray,
                               const Vec3f& origin,
                               const Vec3f& dir) const
{
	float rayDist;
	const auto visibleChunks = chunkStatutManager_.GetVisibleChunks();
	for (auto visibleChunk : visibleChunks)
	{
		if (chunkPosManager_.GetAabb(visibleChunk).IntersectRay(origin, dir, rayDist))
		{
			ray.hitId = visibleChunk;
			ray.hitDist = rayDist;
			return true;
		}
	}
	return false;
}

bool AabbManager::RaycastBlock(Ray& ray,
                               const Vec3f& origin,
                               const Vec3f& dir) const
{
	float rayDist;
	const auto visibleChunks = chunkStatutManager_.GetVisibleChunks();
	for (auto visibleChunk : visibleChunks)
	{
		const Vec3f chunkPos = transform3dManager_.GetPosition(visibleChunk);
		
		for (unsigned x = 0; x < kChunkSize; x++)
		{
			for (unsigned y = 0; y < kChunkSize; y++)
			{
				for (unsigned z = 0; z < kChunkSize; z++)
				{
					const int blockId = chunkContentManager_.GetBlockId(visibleChunk, Vec3i(x, y, z));
					
					if (blockId == 0) continue;
					
					Aabb3d aabb;
					aabb.SetFromCenter(Vec3f(x, y, z) + chunkPos, Vec3f(kCubeHalfSize));
					if (aabb.IntersectRay(dir, origin, rayDist) && rayDist > 0 && rayDist < ray.hitDist)
					{
						ray.hitId = blockId;
						ray.hitDist = rayDist;
						ray.hitPos = Vec3f(x, y, z) + chunkPos * kChunkSize;
						ray.hitAabb = aabb;
					}
				}
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
	const Vec3f chunkPos = transform3dManager_.GetPosition(chunkIndex);

	float rayDist;
	for (size_t x = 0; x < kChunkSize; x++)
	{
		for (size_t y = 0; y < kChunkSize; y++)
		{
			for (size_t z = 0; z < kChunkSize; z++)
			{
				const int blockId = chunkContentManager_.GetBlockId(chunkIndex, Vec3i(x, y, z));
				
				if (blockId == 0) continue;
				
				Aabb3d aabb;
				aabb.SetFromCenter(Vec3f(x, y, z), Vec3f::one / 2);
				if (aabb.IntersectRay(dir, origin, rayDist) && rayDist > 0 && rayDist < ray.hitDist)
				{
					ray.hitId = blockId;
					ray.hitDist = rayDist;
					ray.hitPos = Vec3f(x, y, z) + chunkPos * kChunkSize;
					ray.hitAabb = aabb;
				}
			}
		}
	}
	return ray.hitId != INVALID_INDEX;
}
}
