#pragma once
#include <engine/system.h>
#include <engine/entity.h>

#include "chunk.h"

namespace neko
{
class EntityManager;
class Transform3dManager;
class ChunksManager;
class MinecraftLikeEngine;

class ChunksSystem final : public SystemInterface
{
public:
	explicit ChunksSystem(MinecraftLikeEngine& engine);

	/**
	 * \brief Generate a chunk depend on it position
	 */
	Chunk GenerateChunk(const Vec3i& pos) const;

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
	const float kMaxViewDist_ = 50;
	ChunksManager& chunksManager_;
	Transform3dManager& transform3dManager_;
	EntityManager& entityManager_;
};
}
