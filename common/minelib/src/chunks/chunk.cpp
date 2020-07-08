#include "minelib/chunks/chunk.h"

namespace neko
{
void Chunk::SetBlock(const uint8_t blockId, const Vec3i& pos)
{
	blocksIds_[PosToBlockId(pos)] = blockId;
}

uint8_t Chunk::GetBlockId(const Vec3i& pos) const
{
	return blocksIds_[PosToBlockId(pos)];
}

void Chunk::SetChunkPos(const Vec3f& chunkPos)
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
