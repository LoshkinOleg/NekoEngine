#include "minelib/chunks/chunk.h"

#include "minelib/blocks/block.h"

namespace neko
{
void Chunk::Init()
{
	blocks_.reserve(kChunkBlockCount);

	cube_.Init();
	glGenBuffers(1, &instanceVbo_);
}

void Chunk::Render() const
{
	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, blocks_.size());
}

void Chunk::Destroy()
{
	blocks_.clear();

	cube_.Destroy();
	glDeleteBuffers(1, &instanceVbo_);
}

void Chunk::SetVbo()
{
	glBindBuffer(GL_ARRAY_BUFFER, instanceVbo_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ChunkContent) * blocks_.size(), &blocks_[0], GL_STATIC_DRAW);
	glVertexAttribIPointer(5, 1, GL_SHORT, sizeof(ChunkContent), (void*) offsetof(ChunkContent, blockId));
	glVertexAttribDivisor(5, 1);
	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(6, 1, GL_UNSIGNED_INT, sizeof(ChunkContent), (void*) offsetof(ChunkContent, texId));
	glVertexAttribDivisor(6, 1);
	glEnableVertexAttribArray(6);
}

void Chunk::SetBlock(const std::shared_ptr<Block> block, const Vec3i& pos)
{
	SetBlock(block, PosToBlockId(pos));
}

void Chunk::SetBlock(const std::shared_ptr<Block> block, const BlockId blockId)
{
	if (block->blockTex.sideTexId == 0) return;
	if (blockId > kChunkBlockCount - 1)
	{
		LogError("Block ID out of bounds! ID: " + std::to_string(blockId));
		return;
	}
	
	const auto it = std::find_if(blocks_.begin(), blocks_.end(),
		[blockId](const ChunkContent& content)
		{ return content.blockId == blockId; }); 
	if (it != blocks_.end())
	{
		blocks_[it - blocks_.begin()].texId = BlockTexToBlockHash(block->blockTex);
		return;
	}

	blocks_.emplace_back(blockId, BlockTexToBlockHash(block->blockTex));
}

void Chunk::RemoveBlock(const Vec3i& pos)
{
	RemoveBlock(PosToBlockId(pos));
}

void Chunk::RemoveBlock(const BlockId blockId)
{
	if (blockId < 0 || blockId > kChunkBlockCount - 1)
	{
		LogError("BlockID out of bounds! ID: " + std::to_string(blockId));
		return;
	}
	
	const auto it = std::find_if(blocks_.begin(), blocks_.end(),
		[blockId](const ChunkContent& content)
		{ return content.blockId == blockId; }); 
	if (it != blocks_.end())
	{
		blocks_.erase(it);
	}

	LogError("Block ID is invalid!");
}

void Chunk::SetChunkPos(const Vec3f& chunkPos)
{
	chunkPos_ = chunkPos;
}

Aabb3d Chunk::GetAabb() const
{
	Aabb3d aabb;
	aabb.SetFromCenter(chunkPos_, Vec3f(kChunkSize / 2.0f));
	return aabb;
}
}
