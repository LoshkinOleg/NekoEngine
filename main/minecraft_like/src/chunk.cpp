#include <chunk.h>

namespace neko
{
void Chunk::SetBlock(const uint8_t blockId, const Vec3i& pos)
{
	blocksIds[pos.x + pos.y * kChunkSize + pos.z *kChunkSize * kChunkSize] = blockId;
}

uint8_t Chunk::GetBlockId(const Vec3i& pos)
{
	return blocksIds[pos.x + pos.y * kChunkSize + pos.z *kChunkSize * kChunkSize];
}
}
