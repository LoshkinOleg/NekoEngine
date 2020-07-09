#include "xx_chunk_renderer/chunk.h"

#include "minelib/blocks/block.h"

namespace neko
{
void TestChunk::Init()
{
	blockIds_.reserve(kChunkBlockCount);
	sideTexIds_.reserve(kChunkBlockCount);
	topTexIds_.reserve(kChunkBlockCount);
	bottomTexIds_.reserve(kChunkBlockCount);

	cube_.Init();
	glGenBuffers(4, &instanceVbo_[0]);
}

void TestChunk::Render() const
{
	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, blockIds_.size());
}

void TestChunk::Destroy()
{
	blockIds_.clear();
	sideTexIds_.clear();
	topTexIds_.clear();
	bottomTexIds_.clear();
}

void TestChunk::SetVbo()
{
	glBindBuffer(GL_ARRAY_BUFFER, instanceVbo_[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint16_t) * blockIds_.size(), &blockIds_[0], GL_STATIC_DRAW);
	glVertexAttribIPointer(5, 1, GL_SHORT, sizeof(uint16_t), nullptr);
	glVertexAttribDivisor(5, 1);
	glEnableVertexAttribArray(5);
	
	glBindBuffer(GL_ARRAY_BUFFER, instanceVbo_[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint8_t) * sideTexIds_.size(), &sideTexIds_[0], GL_STATIC_DRAW);
	glVertexAttribIPointer(6, 1, GL_UNSIGNED_BYTE, sizeof(uint8_t), nullptr);
	glVertexAttribDivisor(6, 1);
	glEnableVertexAttribArray(6);
	
	glBindBuffer(GL_ARRAY_BUFFER, instanceVbo_[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint8_t) * topTexIds_.size(), &topTexIds_[0], GL_STATIC_DRAW);
	glVertexAttribIPointer(7, 1, GL_UNSIGNED_BYTE, sizeof(uint8_t), nullptr);
	glVertexAttribDivisor(7, 1);
	glEnableVertexAttribArray(7);
	
	glBindBuffer(GL_ARRAY_BUFFER, instanceVbo_[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint8_t) * bottomTexIds_.size(), &bottomTexIds_[0], GL_STATIC_DRAW);
	glVertexAttribIPointer(8, 1, GL_UNSIGNED_BYTE, sizeof(uint8_t), nullptr);
	glVertexAttribDivisor(8, 1);
	glEnableVertexAttribArray(8);
}

void TestChunk::SetBlock(const Block* block, const Vec3i& pos)
{
	const uint16_t id = PosToBlockId(pos);
	SetBlock(block, id);
}

void TestChunk::SetBlock(const Block* block, const uint16_t blockId)
{
	if (block->sideTexId == 0) return;
	if (blockId > kChunkBlockCount - 1)
	{
		LogError("Block ID out of bounds! ID: " + std::to_string(blockId));
		return;
	}

	const auto it = std::find(blockIds_.begin(), blockIds_.end(), blockId);
	if (it != blockIds_.end())
	{
		const auto index = it - blockIds_.begin();
		sideTexIds_[index] = block->sideTexId;
		topTexIds_[index] = block->topTexId;
		bottomTexIds_[index] = block->bottomTexId;
		return;
	}

	blockIds_.push_back(blockId);
	sideTexIds_.push_back(block->sideTexId);
	topTexIds_.push_back(block->topTexId);
	bottomTexIds_.push_back(block->bottomTexId);
}

void TestChunk::RemoveBlock(const Vec3i& pos)
{
	const uint16_t id = PosToBlockId(pos);
	RemoveBlock(id);
}

void TestChunk::RemoveBlock(const uint16_t blockId)
{
	if (blockId < 0 || blockId > kChunkBlockCount - 1)
	{
		LogError("Block ID out of bounds! ID: " + std::to_string(blockId));
		return;
	}
	const auto it = std::find(blockIds_.begin(), blockIds_.end(), blockId);
	if (it != blockIds_.end())
	{
		blockIds_.erase(it);
		
		const auto index = sideTexIds_.begin() + (it - blockIds_.begin());
		sideTexIds_.erase(index);
		topTexIds_.erase(index);
		bottomTexIds_.erase(index);
	}

	LogError("Block ID is invalid!");
}

void TestChunk::SetChunkPos(const Vec3f& chunkPos)
{
	chunkPos_ = chunkPos;
}

Aabb3d TestChunk::GetAabb() const
{
	Aabb3d aabb;
	aabb.SetFromCenter(chunkPos_, Vec3f(kChunkSize / 2.0f));
	return aabb;
}

uint16_t PosToBlockId(const Vec3i& pos)
{
	return pos.x + pos.y * kChunkSize + pos.z * kChunkSize * kChunkSize;
}

Vec3i BlockIdToPos(const uint16_t pos)
{
	return {pos % kChunkSize, pos / kChunkSize % kChunkSize, pos / (kChunkSize * kChunkSize)};
}
}
