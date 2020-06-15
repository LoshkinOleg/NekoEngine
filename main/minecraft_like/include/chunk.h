#pragma once
#include <cstdint>
#include <array>

#include <mathematics/vector.h>

namespace neko
{
const static int kChunkSize = 16;

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
	
	void SetBlock(const uint8_t blockId, const Vec3i& pos);
	uint8_t GetBlockId(const Vec3i& pos);
private:
	Vec3f chunkPos_;

	uint8_t chunkFlags_ = 0;
	std::array<uint8_t, kChunkSize * kChunkSize * kChunkSize> blocksIds = { 0 };
};
}
