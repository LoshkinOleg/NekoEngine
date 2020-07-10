#pragma once
#include <engine/system.h>
#include <engine/entity.h>

namespace neko
{
class EntityManager;
class Transform3dManager;
class MinecraftLikeEngine;

class ChunkSystem final : public SystemInterface
{
public:
	explicit ChunkSystem(MinecraftLikeEngine& engine);

	/**
	 * \brief Generate a chunk depend on it position
	 */
	void GenerateChunkArray(const Entity index, const Vec3i& pos) const;

	void Init() override;

	/**
	 * \brief Update chunks if they are visible or not and load new chunks
	 */
	void UpdateVisibleChunks() const;

	void Update(seconds dt) override;

	void FixedUpdate() override
	{
	}

	void Destroy() override;

private:
	const float kMaxViewDist_ = 25;
	BlockManager& blockManager_;
	ChunkContentManager& chunkContentManager_;
	ChunkStatusManager& chunkStatusManager_;
	ChunkPosManager& chunkPosManager_;
	ChunkRenderManager& chunkRenderManager_;
	Transform3dManager& transform3dManager_;
	EntityManager& entityManager_;
};
}
