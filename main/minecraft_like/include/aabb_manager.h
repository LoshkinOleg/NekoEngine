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

struct Block
{
	Block() {}
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
	virtual Block RaycastBlock(Vec3f origin, Vec3f dir) = 0;
	virtual Block RaycastBlockInChunk(Vec3f origin, Vec3f dir, Index chunkIndex) = 0;

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
	Block RaycastBlock(Vec3f origin, Vec3f dir) override
	{
		return Block();
	}
	Block RaycastBlockInChunk(Vec3f origin, Vec3f dir, Index chunkIndex) override
	{
		return Block();
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
	Block RaycastBlock(Vec3f origin, Vec3f dir) override;
	Block RaycastBlockInChunk(Vec3f origin, Vec3f dir, Index chunkIndex) override;

private:
	MinecraftLikeEngine& engine_;
	EntityManager& entityManager_;
	ChunksManager& chunksManager_;
	Transform3dManager& transform3dManager_;
};

using AabbLocator = Locator<AabbManagerInterface, NullAabbManager>;
}
