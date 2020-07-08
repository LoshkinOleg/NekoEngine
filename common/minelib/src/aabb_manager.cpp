#include "minelib/aabb_manager.h"

#include "minelib/minecraft_like_engine.h"

#include "engine/transform.h"
#include "minelib/blocks/block.h"

namespace neko
{
AabbManager::AabbManager(MinecraftLikeEngine& engine)
	: engine_(engine),
	  entityManager_(engine.entityManager_),
	  chunksManager_(engine.componentsManagerSystem_.chunkManager_),
	  transform3dManager_(engine.componentsManagerSystem_.transform3dManager_)
{
	AabbLocator::provide(this);
}

bool AabbManager::RaycastChunk(Ray& ray,
                               const Vec3f& origin,
                               const Vec3f& dir) const
{
	float rayDist;
	for (size_t i = 0; i < INIT_ENTITY_NMB; i++)
	{
		if (chunksManager_.GetComponent(i).GetAabb().IntersectRay(origin, dir, rayDist))
		{
			ray.hitId = i;
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
	for (size_t entity = 0; entity < INIT_ENTITY_NMB; entity++)
	{
		if (!engine_.entityManager_.HasComponent(entity, static_cast<EntityMask>(ComponentType::CHUNK))) { continue; }
		Chunk chunk = engine_.componentsManagerSystem_.chunkManager_.GetComponent(entity);
		Vec3f chunkPos = engine_.componentsManagerSystem_.transform3dManager_.GetPosition(entity);

		for (uint16_t i = 0; i < uint16_t(kChunkSize * kChunkSize * kChunkSize); i++)
		{
			//TODO update with new chunk structure
			const uint16_t z = std::floor(i / (kChunkSize * kChunkSize));
			const uint16_t y = std::floor((i - z * kChunkSize * kChunkSize) / kChunkSize);
			const uint16_t x = i % kChunkSize;
			const int blockId = chunk.GetBlockId(Vec3i(x, y, z));
			
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
	return ray.hitId != INVALID_INDEX;
}

bool AabbManager::RaycastBlockInChunk(Ray& ray,
                                      const Vec3f& origin,
                                      const Vec3f& dir,
                                      const Index chunkIndex) const
{
	if (!engine_.entityManager_.HasComponent(chunkIndex, static_cast<EntityMask>(ComponentType::CHUNK))) { return false; }
	const Chunk chunk = engine_.componentsManagerSystem_.chunkManager_.GetComponent(chunkIndex);
	const Vec3f chunkPos = engine_.componentsManagerSystem_.transform3dManager_.GetPosition(chunkIndex);

	float rayDist;
	for (uint16_t i = 0; i < uint16_t(kChunkSize * kChunkSize * kChunkSize); i++)
	{
		const uint16_t z = std::floor(i / (kChunkSize * kChunkSize));
		const uint16_t y = std::floor((i - z * kChunkSize * kChunkSize) / kChunkSize);
		const uint16_t x = i % kChunkSize;
		const int blockId = chunk.GetBlockId(Vec3i(x, y, z));
		
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
	return ray.hitId != INVALID_INDEX;
}
}
