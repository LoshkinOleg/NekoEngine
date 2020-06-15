#include <chunk.h>

namespace neko
{
void Chunk::SetBlock(const uint8_t blockId, const Vec3i& pos)
{
	blocksIds[pos.x + pos.y * chunkSize + pos.z *chunkSize * chunkSize] = blockId;
}

uint8_t Chunk::GetBlockId(const Vec3i& pos)
{
	return blocksIds[pos.x + pos.y * chunkSize + pos.z *chunkSize * chunkSize];
}

Vec3f Chunk::GetChunkPos() const
{
	return chunkPos_;
}

Aabb3d Chunk::GetAabb() const
{
	Aabb3d aabb;
	aabb.SetFromCenter(chunkPos_, Vec3f(chunkSize / 2, chunkSize / 2, chunkSize / 2));
	return aabb;
}
}
