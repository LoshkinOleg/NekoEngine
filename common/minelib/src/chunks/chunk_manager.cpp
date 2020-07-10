#include "minelib/chunks/chunk_manager.h"

namespace neko
{
//-----------------------------------------------------------------------------
// ChunkContentManager
//-----------------------------------------------------------------------------
Index ChunkContentManager::AddComponent(const Entity chunkIndex)
{
    ResizeIfNecessary(components_, chunkIndex, ChunkContentVector{});
    entityManager_.AddComponentType(chunkIndex, static_cast<EntityMask>(ComponentType::CHUNK_CONTENT));
	components_[chunkIndex].reserve(kChunkBlockCount);
    return chunkIndex;
}

void ChunkContentManager::SetBlock(const Entity chunkIndex, const std::shared_ptr<Block> block, const Vec3i& pos)
{
	SetBlock(chunkIndex, block, PosToBlockId(pos));
}

void ChunkContentManager::SetBlock(const Entity chunkIndex, const std::shared_ptr<Block> block, BlockId blockId)
{
	if (block->blockTex.sideTexId == 0) return;
	if (blockId > kChunkBlockCount - 1)
	{
		LogError("Block ID out of bounds! ID: " + std::to_string(blockId));
		return;
	}
	
	const auto it = std::find_if(components_[chunkIndex].begin(), components_[chunkIndex].end(),
		[blockId](const ChunkContent& content)
		{ return content.blockId == blockId; }); 
	if (it != components_[chunkIndex].end())
	{
		components_[chunkIndex][it - components_[chunkIndex].begin()].texId = BlockTexToBlockHash(block->blockTex);
		return;
	}

	components_[chunkIndex].emplace_back(blockId, BlockTexToBlockHash(block->blockTex));
}

void ChunkContentManager::RemoveBlock(const Entity chunkIndex, const Vec3i& pos)
{
	RemoveBlock(chunkIndex, PosToBlockId(pos));
}

void ChunkContentManager::RemoveBlock(const Entity chunkIndex, const BlockId blockId)
{
	if (blockId < 0 || blockId > kChunkBlockCount - 1)
	{
		LogError("BlockID out of bounds! ID: " + std::to_string(blockId));
		return;
	}
	
	const auto it = std::find_if(components_[chunkIndex].begin(), components_[chunkIndex].end(),
		[blockId](const ChunkContent& content)
		{ return content.blockId == blockId; }); 
	if (it != components_[chunkIndex].end())
	{
		components_[chunkIndex].erase(it);
	}

	LogError("Block ID is invalid!");
}

size_t ChunkContentManager::GetChunkSize(const Entity chunkIndex) const
{
	return components_[chunkIndex].size();
}

ChunkContentVector ChunkContentManager::GetBlocks(const Entity chunkIndex) const
{
	return components_[chunkIndex];
}

std::shared_ptr<ChunkContent> ChunkContentManager::GetBlock(const Entity chunkIndex, const Vec3i& pos) const
{
	return GetBlock(chunkIndex, PosToBlockId(pos));
}

std::shared_ptr<ChunkContent> ChunkContentManager::GetBlock(const Entity chunkIndex, BlockId blockId) const
{
	const auto it = std::find_if(components_[chunkIndex].begin(), components_[chunkIndex].end(),
		[blockId](const ChunkContent& content)
		{ return content.blockId == blockId; }); 
	if (it != components_[chunkIndex].end())
	{
		return std::make_shared<ChunkContent>(components_[chunkIndex][it - components_[chunkIndex].begin()]);
	}
	
	LogError("Block ID out of bounds! ID: " + std::to_string(blockId));
	return nullptr;
}

void ChunkContentManager::DestroyComponent(const Entity chunkIndex)
{
	components_[chunkIndex].clear();
    entityManager_.RemoveComponentType(chunkIndex, static_cast<EntityMask>(ComponentType::CHUNK_CONTENT));
}

//-----------------------------------------------------------------------------
// ChunkStatusManager
//-----------------------------------------------------------------------------
void ChunkStatusManager::AddStatus(const Entity entity, ChunkFlag chunkFlag)
{
	components_[entity] |= ChunkMask(chunkFlag);
}

void ChunkStatusManager::RemoveStatus(const Entity entity, ChunkFlag chunkFlag)
{
	components_[entity] &= ~ChunkMask(chunkFlag);
}

bool ChunkStatusManager::HasStatus(const Entity entity, ChunkFlag chunkFlag) const
{
	if (entity >= components_.size())
	{
		std::ostringstream oss;
		oss << "[Error] Accessing entity: " << entity << " while chunk array is of size: " << components_.size();
		LogDebug(oss.str());
		return false;
	}
	return (components_[entity] & ChunkMask(chunkFlag)) == ChunkMask(chunkFlag);
}

std::vector<Index> ChunkStatusManager::GetAccessibleChunks() const
{
	std::vector<Index> accessibleChunks;
	for (size_t index = 0; index < components_.size(); index++)
	{
		if (HasStatus(index, ChunkFlag::ACCESSIBLE))
		{
			accessibleChunks.push_back(index);
		}
	}
	return accessibleChunks;
}

std::vector<Index> ChunkStatusManager::GetVisibleChunks() const
{
	std::vector<Index> visibleChunks;
	for (size_t index = 0; index < components_.size(); index++)
	{
		if (HasStatus(index, ChunkFlag::VISIBLE))
		{
			visibleChunks.push_back(index);
		}
	}
	return visibleChunks;
}

std::vector<Index> ChunkStatusManager::GetLoadedChunks() const
{
	std::vector<Index> loadedChunks;
	for (size_t index = 0; index < components_.size(); index++)
	{
		if (HasStatus(index, ChunkFlag::LOADED))
		{
			loadedChunks.push_back(index);
		}
	}
	return loadedChunks;
}

//-----------------------------------------------------------------------------
// ChunkPosManager
//-----------------------------------------------------------------------------
Aabb3d ChunkPosManager::GetAabb(const Entity chunkIndex) const
{
	const Vec3i chunkPos = components_[chunkIndex];
	return {Vec3f(chunkPos * kChunkSize), Vec3f(kChunkSize / 2.0f)};
}

//-----------------------------------------------------------------------------
// ChunkRenderManager
//-----------------------------------------------------------------------------
ChunkRenderManager::ChunkRenderManager(EntityManager& entityManager, ChunkContentManager& chunkContentManager)
	: ComponentManager<ChunkRender, ComponentType::CHUNK_RENDER>(entityManager),
	  chunkContentManager_(chunkContentManager)
{
	entityManager.RegisterComponentManager(*this);
    ResizeIfNecessary(components_, INIT_ENTITY_NMB - 1, ChunkRender{});
}

Index ChunkRenderManager::AddComponent(const Entity chunkIndex)
{
    ResizeIfNecessary(components_, chunkIndex, ChunkRender{});
    entityManager_.AddComponentType(chunkIndex, static_cast<EntityMask>(ComponentType::CHUNK_RENDER));
	
	components_[chunkIndex].cube.Init();
	glGenBuffers(1, &components_[chunkIndex].vbo);
	
    return chunkIndex;
}

void ChunkRenderManager::SetChunkValues(const Entity chunkIndex)
{
	const auto blocks = chunkContentManager_.GetBlocks(chunkIndex);
	glBindBuffer(GL_ARRAY_BUFFER, components_[chunkIndex].vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ChunkContent) * blocks.size(), &blocks[0], GL_STATIC_DRAW);
	glVertexAttribIPointer(5, 1, GL_SHORT, sizeof(ChunkContent), (void*) offsetof(ChunkContent, blockId));
	glVertexAttribDivisor(5, 1);
	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(6, 1, GL_UNSIGNED_INT, sizeof(ChunkContent), (void*) offsetof(ChunkContent, texId));
	glVertexAttribDivisor(6, 1);
	glEnableVertexAttribArray(6);
}

void ChunkRenderManager::Draw(const Entity chunkIndex) const
{
	return components_[chunkIndex].cube.DrawInstanced(chunkContentManager_.GetChunkSize(chunkIndex));
}

void ChunkRenderManager::DestroyComponent(const Entity chunkIndex)
{
	components_[chunkIndex].cube.Destroy();
	glDeleteBuffers(1, &components_[chunkIndex].vbo);
    entityManager_.RemoveComponentType(chunkIndex, static_cast<EntityMask>(ComponentType::CHUNK_RENDER));
}

//-----------------------------------------------------------------------------
// ChunksViewer
//-----------------------------------------------------------------------------
ChunksViewer::ChunksViewer(
	EntityManager& entityManager,
	ChunkContentManager& chunkContentManager,
	ChunkStatusManager& chunkStatusManager,
	ChunkPosManager& chunkPosManager)
	: entityManager_(entityManager),
	  chunkContentManager_(chunkContentManager),
	  chunkStatusManager_(chunkStatusManager),
	  chunkPosManager_(chunkPosManager)
{
}

void ChunksViewer::DrawImGui(const Entity selectedEntity) const
{
	if (selectedEntity == INVALID_ENTITY)
		return;
	if (entityManager_.HasComponent(selectedEntity, static_cast<EntityMask>(ComponentType::CHUNK_POS)))
	{
		if (ImGui::CollapsingHeader("Chunk"))
		{
			Vec3i chunkPos = chunkPosManager_.GetComponent(selectedEntity);
			ImGui::DragInt3("ChunkPos", &chunkPos[0]);
		}
	}
}
}

