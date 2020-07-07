#pragma once
#include <engine/component.h>
#include <mathematics/aabb.h>

namespace neko
{
const static unsigned int kChunkSize = 16;
using BlocksArray = std::array<uint8_t, kChunkSize * kChunkSize * kChunkSize>;

enum class BlockType : uint8_t
{
	DIRT = 1u << 0u,
	DIAMOND = 1u << 1u,
	STONE = 1u << 2u
};

class ChunkContentManager final : public neko::ComponentManager<BlocksArray, ComponentType::CHUNK_CONTENT>
{
	using ComponentManager::ComponentManager;
public:

	void SetBlock(const Entity chunkIndex, const uint8_t blockId, const Vec3i& pos);

	uint8_t GetBlockId(const Entity chunkIndex, const Vec3i& pos) const;
};

using ChunkMask = std::uint8_t;
enum class ChunkFlag : std::uint8_t
{
	EMPTY = 1u << 0u,
	ACCESSIBLE = 1u << 1u,
	VISIBLE = 1u << 2u,
	LOADED = 1u << 3u
};

class ChunkStatutManager final : public neko::ComponentManager<ChunkMask, ComponentType::CHUNK_STATUT>
{
	using ComponentManager::ComponentManager;
public:
	void AddStatut(Entity entity, ChunkFlag chunkFlag);

	void RemoveStatut(Entity entity, ChunkFlag chunkFlag);

	bool HasStatut(Entity entity, ChunkFlag chunkFlag) const;
};

class ChunkPosManager final : public neko::ComponentManager<Vec3i, ComponentType::CHUNK_POS>
{
	using ComponentManager::ComponentManager;
public:
	Aabb3d GetAabb(const Entity chunkIndex) const;
};

class ChunksViewer
{
public:
	explicit ChunksViewer(
		EntityManager& entityManager,
		ChunkContentManager& chunkContentManager,
		ChunkStatutManager& chunkStatutManager,
		ChunkPosManager& chunkPosManager);

	void DrawImGui(const Entity selectedEntity);

protected:
	EntityManager& entityManager_;
	ChunkContentManager& chunkContentManager_;
	ChunkStatutManager& chunkStatutManager_;
	ChunkPosManager& chunkPosManager_;
};
}
