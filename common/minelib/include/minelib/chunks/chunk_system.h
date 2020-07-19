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

	void Init() override;

	void Update(seconds dt) override;
	void Render() override;

	void FixedUpdate() override
	{
	}

	void Destroy() override;

private:
	/**
	 * \brief Generate a chunk depend on it position
	 */
	Entity GenerateChunkArray(const Vec3i& pos) const;

	void SetChunkOcclusionCulling(Entity chunkIndex) const;

	/**
	 * \brief Update chunks if they are visible or not and load new chunks
	 */
	void UpdateVisibleChunks();
	
	std::mutex mutex_;
	
	BlockManager& blockManager_;
	ChunkManager& chunkManager_;
	EntityManager& entityManager_;

	std::vector<std::function<void()>> scheduledChunks_;
};
}
