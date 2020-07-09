#pragma once
#include "mathematics/aabb.h"
#include "minelib/blocks/block.h"

namespace neko
{
const static uint16_t kChunkSize = 16;
const static uint16_t kChunkBlockCount = kChunkSize * kChunkSize * kChunkSize;

enum class ChunkFlags : uint8_t
{
	LOADED = 1u << 0u,
	EMPTY = 1u << 1u,
	PERSISTENT = 1u << 2u
};

class TestChunk
{
public:
	TestChunk() = default;

	void Init();
	void Render() const;
	void Destroy();

	unsigned GetVao() const { return cube_.VAO; }
	void SetVbo();
	
	void SetBlock(const Block* block, const Vec3i& pos);
	void SetBlock(const Block* block, uint16_t blockId);
	void RemoveBlock(const Vec3i& pos);
	void RemoveBlock(uint16_t blockId);
	
	Vec3f GetChunkPos() const { return chunkPos_; }
	void SetChunkPos(const Vec3f& chunkPos);

	uint32_t GetBlockAmount() const { return blockIds_.size(); }
	std::vector<uint16_t> GetBlockIds() const { return blockIds_; }
	
	Aabb3d GetAabb() const;
private:
	Vec3f chunkPos_;

	uint8_t chunkFlags_ = 0;
	std::vector<uint16_t> blockIds_{};
	std::vector<uint8_t> sideTexIds_{};
	std::vector<uint8_t> topTexIds_{};
	std::vector<uint8_t> bottomTexIds_{};
	
	unsigned instanceVbo_[4]{};
	gl::RenderCuboid cube_{Vec3f::zero, Vec3f(kCubeHalfSize * 2.0f)};
};

static uint16_t PosToBlockId(const Vec3i& pos);
static Vec3i BlockIdToPos(uint16_t pos);
}
