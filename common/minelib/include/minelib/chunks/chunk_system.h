#pragma once
#include <engine/system.h>
#include <engine/entity.h>
#include <mathematics/vector.h>
#include <thread>
#include <mutex>
#include <minelib/chunks/chunk.h>
#include <minelib/chunks/chunk_manager.h>

namespace neko
{
class EntityManager;
class Transform3dManager;
class MinecraftLikeEngine;

class ChunkSystem final : public RenderCommandInterface, public SystemInterface, public DrawImGuiInterface
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

	void DrawImGui() override;
	
private:

	/**
	 * \brief Calculate if a chunk occlude other chunks
	 */
	bool CalculateOcclusionStatus(const ChunkContentVector& chunkContent, ChunkFlag occludeDir) const;
	/**
	 * \brief Generate a chunk depend on it position
	 */
	Entity GenerateChunkContent(Entity newChunkIndex, const Vec3i& pos);

	/**
	 * \brief Calculate if a chunk is occluded
	 */
	void CalculateVisibleStatus(Entity chunkIndex) const;

	/**
	 * \brief Update chunks if they are visible or not and load new chunks
	 */
	void UpdateVisibleChunks();
	
	std::mutex mutex_;
	
	BlockManager& blockManager_;
	ChunkManager& chunkManager_;
	EntityManager& entityManager_;
	MinecraftLikeEngine& engine_;

	std::vector<std::function<void()>> scheduledRenderValues_;
	std::vector<std::unique_ptr<Job>> scheduledGenerationJobs_;
	
};
}
