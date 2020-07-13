#pragma once
#include <engine/component.h>
#include <mathematics/aabb.h>

#include "chunk.h"
#include "chunk_renderer.h"

namespace neko
{
using ChunkContentVector = std::vector<ChunkContent>;

static BlockId PosToBlockId(const Vec3i& pos)
{
	return pos.x + pos.y * kChunkSize + pos.z * kChunkSize * kChunkSize;
}

static Vec3i BlockIdToPos(const BlockId pos)
{
	return {pos % kChunkSize, pos / kChunkSize % kChunkSize, pos / (kChunkSize * kChunkSize)};
}

class ChunkContentManager final : public ComponentManager<ChunkContentVector, ComponentType::CHUNK_CONTENT>
{
	using ComponentManager::ComponentManager;
public:
	Index AddComponent(Entity chunkIndex) override;
	
	void SetBlock(Entity chunkIndex, std::shared_ptr<Block> block, const Vec3i& pos);
	void SetBlock(Entity chunkIndex, std::shared_ptr<Block> block, BlockId blockId);
	void RemoveBlock(Entity chunkIndex, const Vec3i& pos);
	void RemoveBlock(Entity chunkIndex, BlockId blockId);

	size_t GetChunkSize(Entity chunkIndex) const;
	ChunkContentVector GetBlocks(Entity chunkIndex) const;
	std::shared_ptr<ChunkContent> GetBlock(Entity chunkIndex, const Vec3i& pos) const;
	std::shared_ptr<ChunkContent> GetBlock(Entity chunkIndex, BlockId blockId) const;

	void DestroyComponent(Entity chunkIndex) override;
};

using ChunkMask = std::uint8_t;
enum class ChunkFlag : std::uint8_t
{
	EMPTY = 1u << 0u,
	ACCESSIBLE = 1u << 1u,
	VISIBLE = 1u << 2u,
	LOADED = 1u << 3u
};

class ChunkStatusManager final : public ComponentManager<ChunkMask, ComponentType::CHUNK_STATUS>
{
	using ComponentManager::ComponentManager;
public:
	ChunkStatusManager(EntityManager& entityManager, ChunkContentManager& chunkContentManager);
	
	Index AddComponent(Entity chunkIndex) override;

	void AddStatus(Entity entity, ChunkFlag chunkFlag);
	void RemoveStatus(Entity entity, ChunkFlag chunkFlag);
	bool HasStatus(Entity entity, ChunkFlag chunkFlag) const;

	std::vector<Index> GetAccessibleChunks() const;
	std::vector<Index> GetVisibleChunks() const;
	std::vector<Index> GetLoadedChunks() const;
	
private:
	ChunkContentManager& chunkContentManager_;
};

class ChunkPosManager final : public ComponentManager<Vec3i, ComponentType::CHUNK_POS>
{
	using ComponentManager::ComponentManager;
public:
	Aabb3d GetAabb(Entity chunkIndex) const;
};

class ChunkRenderManager final : public ComponentManager<ChunkRender, ComponentType::CHUNK_RENDER>
{
	using ComponentManager::ComponentManager;
public:
	ChunkRenderManager(EntityManager& entityManager, ChunkContentManager& chunkContentManager);

	Index AddComponent(Entity chunkIndex) override;
	
	void Draw(Entity chunkIndex) const;
	void SetChunkValues(Entity chunkIndex);

	void DestroyComponent(Entity chunkIndex) override;
	
private:
	ChunkContentManager& chunkContentManager_;
};

class ChunkManager
{
public:
	explicit ChunkManager(EntityManager& entityManager);
	
	void AddComponent(Entity chunkIndex);
	void DestroyComponent(Entity chunkIndex);

	ChunkContentManager chunkContentManager;
	ChunkStatusManager chunkStatusManager;
	ChunkPosManager chunkPosManager;
	ChunkRenderManager chunkRenderManager;
};

class ChunkViewer
{
public:
	explicit ChunkViewer(EntityManager& entityManager,
		ChunkManager& chunkManager);

	void DrawImGui(Entity selectedEntity) const;

protected:
	EntityManager& entityManager_;
	ChunkManager& chunkManager_;
};
}
