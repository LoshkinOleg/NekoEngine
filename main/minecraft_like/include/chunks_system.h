#pragma once
#include <vector>
#include <engine/system.h>


#include "chunk.h"
#include "engine/entity.h"

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

	void GenerateChunks();

	Chunk GenerateChunk(const Vec3i& pos);

	void Init() override;
	
	void UpdateVisibleChunks();

	void Update(seconds dt) override;
	void FixedUpdate() override {}

	void Destroy() override;

private:
	const float kMaxViewDist_ = 50;
	ChunksManager& chunksManager_;
	Transform3dManager& transform3dManager_;
	EntityManager& entityManager_;
};
}
