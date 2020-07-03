#pragma once
#include <engine/globals.h>

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


//-----------------------------------------------------------------------------
// AabbManagerInterface
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
class AabbManagerInterface
{
public:
	~AabbManagerInterface() = default;
	virtual Index RaycastChunk(Vec3f origin, Vec3f dir) = 0;
	virtual AabbBlock RaycastBlock(Vec3f origin, Vec3f dir) = 0;
	virtual AabbBlock RaycastBlockInChunk(Vec3f origin, Vec3f dir, Index chunkIndex) = 0;

protected:
	const int kInvalidPos_ = -1.0f;
};

//-----------------------------------------------------------------------------
// NullAabbManager
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
class NullAabbManager final : public AabbManagerInterface
{
	Index RaycastChunk(Vec3f origin, Vec3f dir) override
	{
		return INVALID_INDEX;
	}
	AabbBlock RaycastBlock(Vec3f origin, Vec3f dir) override
	{
		return AabbBlock();
	}
	AabbBlock RaycastBlockInChunk(Vec3f origin, Vec3f dir, Index chunkIndex) override
	{
		return AabbBlock();
	}
};

//-----------------------------------------------------------------------------
// AabbManager
//-----------------------------------------------------------------------------
/// \brief Draw gizmos

class AabbManager final : AabbManagerInterface
{
public:
	explicit AabbManager(MinecraftLikeEngine& engine);

	Index RaycastChunk(Vec3f origin, Vec3f dir) override;
	AabbBlock RaycastBlock(Vec3f origin, Vec3f dir) override;
	AabbBlock RaycastBlockInChunk(Vec3f origin, Vec3f dir, Index chunkIndex) override;

private:
	MinecraftLikeEngine& engine_;
	EntityManager& entityManager_;
	ChunksManager& chunksManager_;
	Transform3dManager& transform3dManager_;
};

using AabbLocator = Locator<AabbManagerInterface, NullAabbManager>;
}
