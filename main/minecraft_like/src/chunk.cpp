#include <chunk.h>

namespace neko
{
void Chunk::SetBlock(const uint8_t blockId, const Vec3i& pos)
{
	blocksIds_[pos.x + pos.y * kChunkSize + pos.z * kChunkSize * kChunkSize] = blockId;
}

uint8_t Chunk::GetBlockId(const Vec3i& pos) const
{
	return blocksIds_[pos.x + pos.y * kChunkSize + pos.z * kChunkSize * kChunkSize];
}

Vec3i Chunk::GetChunkPos() const
{
	return chunkPos_;
}

void Chunk::SetChunkPos(const Vec3i& chunkPos)
{
	chunkPos_ = chunkPos;
}

Aabb3d Chunk::GetAabb() const
{
	Aabb3d aabb;
	aabb.SetFromCenter(Vec3f(chunkPos_*kChunkSize), Vec3f(kChunkSize / 2, kChunkSize / 2, kChunkSize / 2));
	return aabb;
}

bool Chunk::IsVisible() const
{
	return visible_;
}

void Chunk::SetVisible(bool visible)
{
	visible_ = visible;
}
}
