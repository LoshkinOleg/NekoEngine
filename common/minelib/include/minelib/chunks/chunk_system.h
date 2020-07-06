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
	//Chunk GenerateChunk(const Vec3i&)

	void Init() override;

	/**
	 * \brief Update chunks if they are visible or not and load new chunks
	 */
	void UpdateVisibleChunks() const;

	void Update(seconds dt) override;

	/**
	 *  \brief Generates a 2d noise map
	 */
	//TODO:: Change name of the function
	static std::array<std::array<int, kChunkSize>, kChunkSize> MapGeneration(Vec2<int> offset, int chunkSize, int chunkHeight, int seed, float frequency, int octaves);

	void FixedUpdate() override
	{
	}

	void Destroy() override;

private:
	const float kMaxViewDist_ = 25;
	ChunksManager& chunksManager_;
	Transform3dManager& transform3dManager_;
	EntityManager& entityManager_;

	Vec2<int> offset{0,0};
	int seed = 0;
	float frequency = 1;
	int octaves = 2;
	
};
}
