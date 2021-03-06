#pragma once
#include <engine/component.h>
#include <mathematics/aabb.h>

#include "chunk.h"
#include "chunk_renderer.h"

#ifdef EASY_PROFILE_USE
#include <easy/profiler.h>
#endif

namespace neko
{

static BlockId PosToBlockId(const Vec3i& pos)
{
	return pos.x + pos.y * kChunkSize + pos.z * kChunkSize * kChunkSize;
}

static Vec3i BlockIdToPos(const BlockId pos)
{
	return {pos % kChunkSize, pos / kChunkSize % kChunkSize, pos / (kChunkSize * kChunkSize)};
}

struct ChunkContentVector
{
	std::vector<ChunkContent> blocks;

	ChunkContentVector();

	void SetBlock(const Block& block, const Vec3i& pos);

	void SetBlock(const Block& block, BlockId blockId);

	void FillOfBlock(const Block& block);

	void RemoveBlock(const Vec3i& pos);

	void RemoveBlock(const BlockId blockId);

	std::vector<ChunkContent> GetBlocks() const;

	ChunkContent& GetBlock(const Vec3i& pos);

	ChunkContent& GetBlock(BlockId blockId);

	bool HasBlockAt(const Vec3i& pos) const;

	void CalculateBlockOcclusion();
	
};

class ChunkContentManager final : public ComponentManager<
		ChunkContentVector, ComponentType::CHUNK_CONTENT>
{
	using ComponentManager::ComponentManager;
public:
	Index AddComponent(Entity chunkIndex) override;
	
	void SetBlock(Entity chunkIndex, const Block& block, const Vec3i& pos);
	void SetBlock(Entity chunkIndex, const Block& block, BlockId blockId);

	void FillOfBlock(Entity chunkIndex, const Block& block);

	void FillOfBlocks(Entity chunkIndex, const ChunkContentVector& chunkContentVector);

	void RemoveBlock(Entity chunkIndex, const Vec3i& pos);

	void RemoveBlock(Entity chunkIndex, BlockId blockId);

	size_t GetChunkSize(Entity chunkIndex);

	std::vector<ChunkContent> GetBlocks(Entity chunkIndex);
	
	ChunkContent& GetBlock(Entity chunkIndex, const Vec3i& pos);
	ChunkContent& GetBlock(Entity chunkIndex, BlockId blockId);

	void DestroyComponent(Entity chunkIndex) override;

private:
	std::mutex mutex_;
};

using ChunkMask = std::uint16_t;

enum class ChunkFlag : std::uint16_t
{
	EMPTY = 1u << 0u,
	ACCESSIBLE = 1u << 1u,
	VISIBLE = 1u << 2u,
	LOADED = 1u << 3u,
	OCCLUDE_DOWN = 1u << 4u,
	OCCLUDE_UP = 1u << 5u,
	OCCLUDE_RIGHT = 1u << 6u,
	OCCLUDE_LEFT = 1u << 7u,
	OCCLUDE_FRONT = 1u << 8u,
	OCCLUDE_BACK = 1u << 9u,
	OCCLUDED = 1u << 10u,
	DIRTY = 1u << 11u
};

class ChunkStatusManager final : public ComponentManager<ChunkMask, ComponentType::CHUNK_STATUS>
{
	using ComponentManager::ComponentManager;
public:
	ChunkStatusManager(EntityManager& entityManager, ChunkContentManager& chunkContentManager);

	Index AddComponent(Entity chunkIndex) override;

	void AddStatus(Entity entity, ChunkFlag chunkFlag);

	void AddStatus(Entity entity, ChunkMask chunkMask);

	void RemoveStatus(Entity entity, ChunkFlag chunkFlag);

	bool HasStatus(Entity entity, ChunkFlag chunkFlag);

	std::vector<Index> GetAccessibleChunks();

	std::vector<Index> GetRenderedChunks();
	
	std::vector<Index> GetDirtyChunks();
	
	std::vector<Index> GetLoadedChunks();

private:
	ChunkContentManager& chunkContentManager_;
	std::mutex mutex_;
};

class ChunkPosManager final : public ComponentManager<Vec3i, ComponentType::CHUNK_POS>
{
	using ComponentManager::ComponentManager;
public:
	Vec3i GetPositon(Entity chunkIndex);

	void SetPositon(Entity chunkIndex, const Vec3i& chunkPos);

	Aabb3d GetAabb(Entity chunkIndex) const;

	Entity GetChunkAtPos(const Vec3i& chunkPos);

private:
	std::mutex mutex_;
};

class ChunkRenderManager final : public ComponentManager<ChunkRender, ComponentType::CHUNK_RENDER>
{
	using ComponentManager::ComponentManager;
public:
	ChunkRenderManager(EntityManager& entityManager, ChunkContentManager& chunkContentManager);

	Index AddComponent(Entity chunkIndex) override;

	void Init(Entity chunkIndex);

	void Draw(Entity chunkIndex);

	void SetChunkValues(const Entity chunkIndex, ChunkContentVector chunkContentVector);

	void SetChunkValues(Entity chunkIndex, std::vector<ChunkContent> chunkContentVector);

	void DestroyComponent(Entity chunkIndex) override;

private:
	ChunkContentManager& chunkContentManager_;
	std::mutex mutex_;
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
	explicit ChunkViewer(
		EntityManager& entityManager,
		ChunkManager& chunkManager);

	void DrawImGui(Entity selectedEntity) const;

protected:
	EntityManager& entityManager_;
	ChunkManager& chunkManager_;
};
}
