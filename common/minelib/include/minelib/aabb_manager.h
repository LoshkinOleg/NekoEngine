#pragma once
#include <engine/globals.h>


#include "mathematics/aabb.h"
#include "mathematics/vector.h"
#include "utilities/service_locator.h"

namespace neko
{
class Transform3dManager;
class EntityManager;
class ChunksManager;
class MinecraftLikeEngine;

struct AabbBlock
{
	AabbBlock() {}
	Vec3f blockPos = Vec3f(0.0f);
	uint8_t blockType = 0;
};

struct Ray
{
	Index hitId = INVALID_INDEX;
	Vec3f hitPos;
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
	virtual bool RaycastBlock(Ray& ray, const Vec3f& origin, const Vec3f& dir) const = 0;
	virtual bool RaycastBlockInChunk(Ray& ray, const Vec3f& origin, const Vec3f& dir, Index chunkIndex) const = 0;

protected:
	const int kInvalidPos_ = -1.0f;
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
	{
		return false;
	}

	bool RaycastBlock([[maybe_unused]] Ray& ray,
	                  [[maybe_unused]] const Vec3f& origin,
	                  [[maybe_unused]] const Vec3f& dir) const override
	{
		return false;
	}

	bool RaycastBlockInChunk([[maybe_unused]] Ray& ray,
	                         [[maybe_unused]] const Vec3f& origin,
	                         [[maybe_unused]] const Vec3f& dir,
	                         [[maybe_unused]] Index chunkIndex) const override
	{
		return false;
	}
};

//-----------------------------------------------------------------------------
// AabbManager
//-----------------------------------------------------------------------------
/// \brief Draw gizmos

class AabbManager final : IAabbManager
{
public:
	explicit AabbManager(MinecraftLikeEngine& engine);

	bool RaycastChunk(Ray& ray,
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
	ChunksManager& chunksManager_;
	Transform3dManager& transform3dManager_;
};

using AabbLocator = Locator<IAabbManager, NullAabbManager>;
}
