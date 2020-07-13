#pragma once
#include <vector>
#include <engine/globals.h>
#include <mathematics/vector.h>
#include <utilities/service_locator.h>

#include "chunks/chunk_manager.h"
#include "mathematics/aabb.h"

namespace neko
{
class ChunkStatusManager;
class ChunkContentManager;
class ChunkPosManager;
class Transform3dManager;
class EntityManager;
class MinecraftLikeEngine;

struct Ray
{
	Index hitId = INVALID_INDEX;
	Entity hitChunk = INVALID_ENTITY;
	Aabb3d hitAabb;
	float hitDist = std::numeric_limits<float>::max();
};

//-----------------------------------------------------------------------------
// IAabbManager
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
class IAabbManager
{
public:
	virtual ~IAabbManager() = default;
	virtual bool RaycastChunk(Ray& ray, const Vec3f& origin, const Vec3f& dir) const = 0;
	virtual std::vector<Index> RaycastChunks(const Vec3f& origin, const Vec3f& dir) const = 0;
	virtual bool RaycastBlock(Ray& ray, const Vec3f& origin, const Vec3f& dir) const = 0;
	virtual bool RaycastBlockInChunk(Ray& ray, const Vec3f& origin, const Vec3f& dir, Index chunkIndex) const = 0;

protected:
	const int kInvalidPos_ = -1;
};

//-----------------------------------------------------------------------------
// NullAabbManager
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
class NullAabbManager final : public IAabbManager
{
	bool RaycastChunk([[maybe_unused]] Ray& ray,
	                  [[maybe_unused]] const Vec3f& origin,
	                  [[maybe_unused]] const Vec3f& dir) const override
	{ return false; }
	
	std::vector<Index> RaycastChunks(
		[[maybe_unused]] const Vec3f& origin,
		[[maybe_unused]] const Vec3f& dir) const override
	{ return {}; }

	bool RaycastBlock([[maybe_unused]] Ray& ray,
	                  [[maybe_unused]] const Vec3f& origin,
	                  [[maybe_unused]] const Vec3f& dir) const override
	{ return false; }

	bool RaycastBlockInChunk([[maybe_unused]] Ray& ray,
	                         [[maybe_unused]] const Vec3f& origin,
	                         [[maybe_unused]] const Vec3f& dir,
	                         [[maybe_unused]] Index chunkIndex) const override
	{ return false; }
};

//-----------------------------------------------------------------------------
// AabbManager
//-----------------------------------------------------------------------------
class AabbManager final : public IAabbManager
{
public:
	explicit AabbManager(MinecraftLikeEngine& engine);

	bool RaycastChunk(Ray& ray,
	                  const Vec3f& origin,
	                  const Vec3f& dir) const override;
	std::vector<Index> RaycastChunks(
		const Vec3f& origin,
		const Vec3f& dir) const override;
	bool RaycastBlock(Ray& ray,
	                  const Vec3f& origin,
	                  const Vec3f& dir) const override;
	bool RaycastBlockInChunk(Ray& ray,
	                         const Vec3f& origin,
	                         const Vec3f& dir,
	                         Index chunkIndex) const override;

private:
	MinecraftLikeEngine& engine_;
	EntityManager& entityManager_;
	ChunkManager& chunkManager_;
	Transform3dManager& transform3dManager_;
};

using AabbLocator = Locator<IAabbManager, NullAabbManager>;
}
