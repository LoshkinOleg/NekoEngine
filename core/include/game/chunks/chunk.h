#pragma once
#include <cstdint>
#include "../../mathematics/vector.h"

namespace neko::game
{
const static int chunkSize = 16;

enum class ChunkFlags : uint8_t
{
	LOADED = 1u << 0u,
	EMPTY = 1u << 1u,
	PERSISTENT = 1u << 2u
};

class Chunk
{
public:
	void SetBlock(const uint8_t blockId, const int x, const int y, const int z)
	{
		blocksIds[x + y * chunkSize + z *chunkSize * chunkSize] = blockId;
	}
	
	uint8_t GetBlockId(const int x, const int y, const int z)
	{
		return blocksIds[x + y * chunkSize + z *chunkSize * chunkSize];
	}
private:
	Vec3f chunkPos;

	uint8_t chunkFlags = 0;
	std::array<uint8_t, chunkSize * chunkSize * chunkSize> blocksIds = {};
};
}
