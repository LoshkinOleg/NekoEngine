#pragma once
#include <cstdint>
#include <array>

#include <mathematics/vector.h>
#include <mathematics/aabb.h>

namespace neko
{
const static unsigned int kChunkSize = 16;

enum class ChunkFlags : uint8_t
{
	EMPTY = 1u << 0u,
	VISIBLE = 1u << 1u,
	LOADED = 1u << 2u
};

class Chunk
{
public:
	Chunk() = default;

	void SetBlock(const uint8_t blockId, const Vec3i& pos);

	uint8_t GetBlockId(const Vec3i& pos) const;

	Vec3i GetChunkPos() const;

	void SetChunkPos(const Vec3i& chunkPos);

	Aabb3d GetAabb() const;

	bool IsVisible() const;

	void SetVisible(bool visible);

private:
	Vec3i chunkPos_;
	bool visible_ = false;

	ChunkFlags chunkFlags_ = ChunkFlags::EMPTY;
	std::array<uint8_t, kChunkSize * kChunkSize * kChunkSize> blocksIds_ = {0};
};
}
