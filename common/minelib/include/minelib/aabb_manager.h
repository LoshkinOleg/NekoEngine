#pragma once
#include <vector>
#include <engine/globals.h>
#include <mathematics/vector.h>
#include <utilities/service_locator.h>


namespace neko
{
class ChunkContentManager;
class ChunkPosManager;
class Transform3dManager;
class EntityManager;
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
	virtual AabbBlock RaycastBlock(const Vec3f& origin, const Vec3f& dir) = 0;

	/**
	 * \brief Return nearest block in the raycast a specified chunk
	 * \param origin : origin of the raycast
	 * \param chunkIndex : index of the raycasted chunk
	 * \return
	 */
	virtual AabbBlock RaycastBlockInChunk(const Vec3f& origin, const Vec3f& dir, const Index chunkIndex) = 0;

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

	AabbBlock RaycastBlock(const Vec3f& origin, const Vec3f& dir) override
	{
		return AabbBlock();
	}

	AabbBlock RaycastBlockInChunk(const Vec3f& origin, const Vec3f& dir, const Index chunkIndex) override
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

	std::vector<Index> RaycastChunk(const Vec3f& origin, const Vec3f& dir) override;

	AabbBlock RaycastBlock(const Vec3f& origin, const Vec3f& dir) override;

	AabbBlock RaycastBlockInChunk(const Vec3f& origin, const Vec3f& dir, const Index chunkIndex) override;

private:
	MinecraftLikeEngine& engine_;
	EntityManager& entityManager_;
	ChunkPosManager& chunkPosManager_;
	ChunkContentManager& chunkContentManager_;
	Transform3dManager& transform3dManager_;
};

using AabbLocator = Locator<AabbManagerInterface, NullAabbManager>;
}
