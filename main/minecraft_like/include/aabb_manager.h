#pragma once
#include <vector>
#include <engine/globals.h>
#include <mathematics/vector.h>
#include <utilities/service_locator.h>

namespace neko
{
class Transform3dManager;
class EntityManager;
class ChunksManager;
class MinecraftLikeEngine;

struct Block
{
	Block()
	{
	}

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
	/**
	 * \brief Return all chunk in the raycast
	 * \param origin : origin of the raycast
	 * \param dir : direction of the raycast
	 * \return 
	 */
	virtual std::vector<Index> RaycastChunk(const Vec3f& origin, const Vec3f& dir) = 0;

	/**
	 * \brief Return the nearest block in the raycast
	 * \param origin : origin of the raycast
	 * \param dir : direction of the raycast
	 * \return
	 */
	virtual Block RaycastBlock(const Vec3f& origin, const Vec3f& dir) = 0;

	/**
	 * \brief Return nearest block in the raycast a specified chunk
	 * \param origin : origin of the raycast
	 * \param chunkIndex : index of the raycasted chunk
	 * \return
	 */
	virtual Block RaycastBlockInChunk(const Vec3f& origin, const Vec3f& dir, const Index chunkIndex) = 0;

protected:
	~AabbManagerInterface() = default;

	const int kInvalidPos_ = -1;
};

//-----------------------------------------------------------------------------
// NullAabbManager
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
class NullAabbManager final : public AabbManagerInterface
{
	std::vector<Index> RaycastChunk(const Vec3f& origin, const Vec3f& dir) override
	{
		return std::vector<Index>();
	}

	Block RaycastBlock(const Vec3f& origin, const Vec3f& dir) override
	{
		return Block();
	}

	Block RaycastBlockInChunk(const Vec3f& origin, const Vec3f& dir, const Index chunkIndex) override
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

	std::vector<Index> RaycastChunk(const Vec3f& origin, const Vec3f& dir) override;

	Block RaycastBlock(const Vec3f& origin, const Vec3f& dir) override;

	Block RaycastBlockInChunk(const Vec3f& origin, const Vec3f& dir, const Index chunkIndex) override;

private:
	MinecraftLikeEngine& engine_;
	EntityManager& entityManager_;
	ChunksManager& chunksManager_;
	Transform3dManager& transform3dManager_;
};

using AabbLocator = Locator<AabbManagerInterface, NullAabbManager>;
}
