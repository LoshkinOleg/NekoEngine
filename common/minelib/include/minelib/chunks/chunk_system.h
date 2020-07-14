#pragma once
#include <engine/system.h>
#include <engine/entity.h>
#include <mathematics/vector.h>
#include <thread>
#include <mutex>

namespace neko
{
class EntityManager;
class Transform3dManager;
class MinecraftLikeEngine;

class ChunkSystem final : public RenderCommandInterface, public SystemInterface
{
public:
	explicit ChunkSystem(MinecraftLikeEngine& engine);

	/**
	 * \brief Generate a chunk depend on it position
	 */
	void GenerateChunkArray(const Vec3i& pos);

	void Init() override;

	/**
	 * \brief Update chunks if they are visible or not and load new chunks
	 */
	void UpdateVisibleChunks();

	void Update(seconds dt) override;
	void Render() override;

	void FixedUpdate() override
	{
	}

	void Destroy() override;

private:

	std::mutex mutex_;
	
	const float kMaxViewDist_ = 32;
	const float kHeighChunktLimit_ = 16;
	BlockManager& blockManager_;
	ChunkManager& chunkManager_;
	Transform3dManager& transform3dManager_;
	EntityManager& entityManager_;

	std::vector<Job> scheduledChunks_;
};
}
