#pragma once

namespace neko
{
const static uint16_t kChunkSize = 4;

enum class ChunkFlags : uint8_t
{
	LOADED = 1u << 0u,
	EMPTY = 1u << 1u,
	PERSISTENT = 1u << 2u
};

class Chunk
{
public:
	Chunk() = default;
	
	void SetBlock(uint8_t blockId, const Vec3i& pos);
	
	uint8_t GetBlockId(const Vec3i& pos) const;
	
	Vec3f GetChunkPos() const { return chunkPos_; }
	void SetChunkPos(const Vec3f& chunkPos);
	
	Aabb3d GetAabb() const;
private:
	Vec3f chunkPos_;

	uint8_t chunkFlags_ = 0;
	std::array<uint8_t, kChunkSize * kChunkSize * kChunkSize> blocksIds_ = { 0 };
};

static uint16_t PosToBlockId(const Vec3i& pos)
{
	return pos.x + pos.y * kChunkSize + pos.z * kChunkSize * kChunkSize;
}
}
