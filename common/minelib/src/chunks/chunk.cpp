#include <minelib/chunks/chunk.h>

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

Vec3f Chunk::GetChunkPos() const
{
	return chunkPos_;
}

void Chunk::SetChunkPos(Vec3f chunkPos)
{
	chunkPos_ = chunkPos;
}

Aabb3d Chunk::GetAabb() const
{
	Aabb3d aabb;
	aabb.SetFromCenter(chunkPos_, Vec3f(kChunkSize / 2, kChunkSize / 2, kChunkSize / 2));
	return aabb;
}
}
