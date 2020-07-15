#pragma once
#include "mathematics/aabb.h"
#include "minelib/blocks/block.h"
#include "gl/shape.h"

namespace neko
{
const static float kMaxViewDist = 128;
const static float kHeightChunkLimit = 16;
const static uint16_t kChunkSize = 16;
const static uint16_t kChunkBlockCount = kChunkSize * kChunkSize * kChunkSize;
using BlockId = uint16_t;

struct ChunkContent
{
	explicit  ChunkContent(const BlockId bId = 0, const TextureHash tId = 0)
		: blockId(bId), texId(tId) {}

	BlockId blockId = 0;
	TextureHash texId = 0;
};

class Chunk
{
public:
	Chunk() = default;

	void Init();
	void Render() const;
	void Destroy();

	unsigned GetVao() const { return cube_.vao; }
	void SetVbo();
	
	void SetBlock(std::shared_ptr<Block> block, const Vec3i& pos);
	void SetBlock(std::shared_ptr<Block> block, BlockId blockId);
	void RemoveBlock(const Vec3i& pos);
	void RemoveBlock(BlockId blockId);
	
	Vec3f GetChunkPos() const { return chunkPos_; }
	void SetChunkPos(const Vec3f& chunkPos);

	size_t GetBlockAmount() const { return blocks_.size(); }
	std::vector<ChunkContent> GetBlockIds() const { return blocks_; }
	
	Aabb3d GetAabb() const;
	
private:
	Vec3f chunkPos_;

	std::vector<ChunkContent> blocks_{};
	
	unsigned instanceVbo_ = 0;
	gl::RenderCuboid cube_{Vec3f::zero, Vec3f(kCubeHalfSize * 2.0f)};
};
}
