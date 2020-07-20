#include "minelib/chunks/chunk_manager.h"

#include "mathematics/aabb.h"
#include "minelib/frustum.h"
#include "minelib/gizmos_renderer.h"

namespace neko
{
//-----------------------------------------------------------------------------
// ChunkContentVector
//-----------------------------------------------------------------------------
ChunkContentVector::ChunkContentVector()
{
	blocks.reserve(kChunkBlockCount);
}

void ChunkContentVector::SetBlock(const Block& block,
	const Vec3i& pos)
{
	SetBlock(block, PosToBlockId(pos));
}

void ChunkContentVector::SetBlock(const Block& block,
	BlockId blockId)
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("ChunkContentVector::SetBlock", profiler::colors::Blue300);
#endif
	if (block.blockTex.sideTexId == 0) return;
	if (blockId > kChunkBlockCount - 1)
	{
		LogError("Block ID out of bounds! ID: " + std::to_string(blockId));
		return;
	}

	const auto it = std::find_if(blocks.begin(),
	                             blocks.end(),
	                             [blockId](const ChunkContent& content)
	                             {
		                             return content.blockId == blockId;
	                             });
	if (it != blocks.end())
	{
		blocks[it - blocks.begin()].texId = BlockTexToTexHash(block.blockTex);
		return;
	}

	blocks.emplace_back(blockId, BlockTexToTexHash(block.blockTex));
}

void ChunkContentVector::FillOfBlock(const Block& block)
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("ChunkContentManager::FillOfBlock", profiler::colors::Blue200);
#endif
	if (block.blockTex.sideTexId == 0) return;
	blocks.clear();
	blocks.reserve(kChunkBlockCount);
	for (BlockId blockId = 0; blockId < kChunkBlockCount; blockId++)
	{
		const ChunkContent content = ChunkContent(blockId, BlockTexToTexHash(block.blockTex));
		blocks.emplace_back(blockId, BlockTexToTexHash(block.blockTex));
	}
}

void ChunkContentVector::RemoveBlock(const Vec3i& pos)
{
	RemoveBlock(PosToBlockId(pos));
}

void ChunkContentVector::RemoveBlock(const BlockId blockId)
{
	if (blockId < 0 || blockId > kChunkBlockCount - 1)
	{
		LogError("BlockID out of bounds! ID: " + std::to_string(blockId));
		return;
	}

	const auto it = std::find_if(blocks.begin(),
	                             blocks.end(),
	                             [blockId](const ChunkContent& content)
	                             {
		                             return content.blockId == blockId;
	                             });
	if (it != blocks.end())
	{
		blocks.erase(it);
	}

	LogError("Block ID is invalid!");
}

std::vector<ChunkContent> ChunkContentVector::GetBlocks() const
{
	return blocks;
}

std::shared_ptr<ChunkContent> ChunkContentVector::GetBlock(
	const Vec3i& pos)
{
	return GetBlock(PosToBlockId(pos));
}

std::shared_ptr<ChunkContent> ChunkContentVector::GetBlock(
	BlockId blockId)
{
	if (blockId < 0 || blockId > kChunkBlockCount - 1)
	{
		LogError("BlockID out of bounds! ID: " + std::to_string(blockId));
		return nullptr;
	}
	const auto it = std::find_if(blocks.begin(),
	                             blocks.end(),
	                             [blockId](const ChunkContent& content)
	                             {
		                             return content.blockId == blockId;
	                             });
	if (it != blocks.end())
	{
		return std::make_shared<ChunkContent>(
			blocks[it - blocks.begin()]);
	}
}

bool ChunkContentVector::HasBlockAt(const Vec3i& pos) const
{
	if (pos.x < 0 || pos.x >= kChunkSize || pos.y < 0 || pos.y >= kChunkSize || pos.z < 0 || pos.z
		>= kChunkSize)
	{
		return false;
	}
	BlockId blockId = PosToBlockId(pos);
	if (blockId < 0 || blockId > kChunkBlockCount - 1)
	{
		LogError("BlockID out of bounds! ID: " + std::to_string(blockId));
		return false;
	}
	const auto it = std::find_if(blocks.begin(),
	                             blocks.end(),
	                             [blockId](const ChunkContent& content)
	                             {
		                             return content.blockId == blockId;
	                             });
	if (it != blocks.end())
	{
		return true;
	}
	return false;
}

void ChunkContentVector::CalculateBlockOcclusion()
{
	std::vector<ChunkContent> visibleBlocks;
	for (auto block : blocks)
	{
		Vec3i blockPos = BlockIdToPos(block.blockId);
		if (!HasBlockAt(blockPos + Vec3i::up) ||
			!HasBlockAt(blockPos + Vec3i::down) ||
			!HasBlockAt(blockPos + Vec3i::right) ||
			!HasBlockAt(blockPos + Vec3i::left) ||
			!HasBlockAt(blockPos + Vec3i::forward) ||
			!HasBlockAt(blockPos + Vec3i::back))
		{
			visibleBlocks.push_back(block);
		}
	}
	blocks = visibleBlocks;
}

//-----------------------------------------------------------------------------
// ChunkContentManager
//-----------------------------------------------------------------------------
Index ChunkContentManager::AddComponent(const Entity chunkIndex)
{
	std::lock_guard<std::mutex> lock(mutex_);
	ResizeIfNecessary(components_, chunkIndex, ChunkContentVector{});
	entityManager_.AddComponentType(chunkIndex,
	                                static_cast<EntityMask>(ComponentType::CHUNK_CONTENT));
	components_[chunkIndex].blocks.reserve(kChunkBlockCount);
	return chunkIndex;
}

void ChunkContentManager::SetBlock(const Entity chunkIndex, const Block& block, const Vec3i& pos)
{
	SetBlock(chunkIndex, block, PosToBlockId(pos));
}

void ChunkContentManager::SetBlock(const Entity chunkIndex, const Block& block, BlockId blockId)
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("ChunkContentManager::SetBlock");
#endif
	if (block.blockTex.sideTexId == 0) return;
	if (blockId > kChunkBlockCount - 1)
	{
		LogError("Block ID out of bounds! ID: " + std::to_string(blockId));
		return;
	}

	std::lock_guard<std::mutex> lock(mutex_);
	const auto it = std::find_if(components_[chunkIndex].blocks.begin(),
	                             components_[chunkIndex].blocks.end(),
	                             [blockId](const ChunkContent& content)
	                             {
		                             return content.blockId == blockId;
	                             });
	if (it != components_[chunkIndex].blocks.end())
	{
		components_[chunkIndex].blocks[it - components_[chunkIndex].blocks.begin()].texId =
			BlockTexToTexHash(block.blockTex);
		return;
	}

	components_[chunkIndex].blocks.emplace_back(blockId, BlockTexToTexHash(block.blockTex));
}

void ChunkContentManager::FillOfBlock(const Entity chunkIndex, const Block& block)
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("ChunkContentManager::FillOfBlock");
#endif
	if (block.blockTex.sideTexId == 0) return;
	std::lock_guard<std::mutex> lock(mutex_);
	components_[chunkIndex].blocks.clear();
	components_[chunkIndex].blocks.reserve(kChunkBlockCount);
	for (BlockId blockId = 0; blockId < kChunkBlockCount; blockId++)
	{
		const ChunkContent content = ChunkContent(blockId, BlockTexToTexHash(block.blockTex));
		components_[chunkIndex].blocks.emplace_back(blockId, BlockTexToTexHash(block.blockTex));
	}
}

void ChunkContentManager::FillOfBlocks(
	const Entity chunkIndex,
	const ChunkContentVector& chunkContentVector)
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("ChunkContentManager::FillOfBlocks");
#endif
	std::lock_guard<std::mutex> lock(mutex_);
	components_[chunkIndex].blocks.clear();
	components_[chunkIndex].blocks.reserve(kChunkBlockCount);
	components_[chunkIndex] = chunkContentVector;
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

	std::lock_guard<std::mutex> lock(mutex_);
	const auto it = std::find_if(components_[chunkIndex].blocks.begin(),
	                             components_[chunkIndex].blocks.end(),
	                             [blockId](const ChunkContent& content)
	                             {
		                             return content.blockId == blockId;
	                             });
	if (it != components_[chunkIndex].blocks.end())
	{
		components_[chunkIndex].blocks.erase(it);
		return;
	}

	LogError("Block ID is invalid!");
}

size_t ChunkContentManager::GetChunkSize(const Entity chunkIndex)
{
	std::lock_guard<std::mutex> lock(mutex_);
	return components_[chunkIndex].blocks.size();
}

std::vector<ChunkContent> ChunkContentManager::GetBlocks(const Entity chunkIndex)
{
	std::lock_guard<std::mutex> lock(mutex_);
	return components_[chunkIndex].blocks;
}

ChunkContent& ChunkContentManager::GetBlock(const Entity chunkIndex, const Vec3i& pos)
{
	return GetBlock(chunkIndex, PosToBlockId(pos));
}

ChunkContent& ChunkContentManager::GetBlock(const Entity chunkIndex, BlockId blockId)
{
	std::lock_guard<std::mutex> lock(mutex_);
	const auto it = std::find_if(components_[chunkIndex].blocks.begin(),
	                             components_[chunkIndex].blocks.end(),
	                             [blockId](const ChunkContent& content)
	                             {
		                             return content.blockId == blockId;
	                             });
	if (it != components_[chunkIndex].blocks.end())
	{
		return components_[chunkIndex].blocks[it - components_[chunkIndex].blocks.begin()];
	}

	LogError("Block ID out of bounds! ID: " + std::to_string(blockId));
	return ChunkContent();
}

void ChunkContentManager::DestroyComponent(const Entity chunkIndex)
{
	components_[chunkIndex].blocks.clear();
	entityManager_.RemoveComponentType(chunkIndex,
	                                   static_cast<EntityMask>(ComponentType::CHUNK_CONTENT));
}

//-----------------------------------------------------------------------------
// ChunkStatusManager
//-----------------------------------------------------------------------------
ChunkStatusManager::ChunkStatusManager(
	EntityManager& entityManager,
	ChunkContentManager& chunkContentManager)
	: ComponentManager<ChunkMask, ComponentType::CHUNK_STATUS>(entityManager),
	  chunkContentManager_(chunkContentManager)
{
}

Index ChunkStatusManager::AddComponent(const Entity chunkIndex)
{
	mutex_.lock();
	ResizeIfNecessary(components_, chunkIndex, ChunkMask{});
	mutex_.unlock();
	entityManager_.AddComponentType(chunkIndex,
	                                static_cast<EntityMask>(ComponentType::CHUNK_STATUS));
	AddStatus(chunkIndex, ChunkFlag::VISIBLE);
	AddStatus(chunkIndex, ChunkFlag::LOADED);
	if (chunkContentManager_.GetChunkSize(chunkIndex) == 0)
		AddStatus(chunkIndex, ChunkFlag::EMPTY);

	return chunkIndex;
}

void ChunkStatusManager::AddStatus(const Entity entity, ChunkFlag chunkFlag)
{
	std::lock_guard<std::mutex> lock(mutex_);
	components_[entity] |= ChunkMask(chunkFlag);
}

void ChunkStatusManager::AddStatus(const Entity entity, ChunkMask chunkMask)
{
	std::lock_guard<std::mutex> lock(mutex_);
	components_[entity] |= chunkMask;
}

void ChunkStatusManager::RemoveStatus(const Entity entity, ChunkFlag chunkFlag)
{
	std::lock_guard<std::mutex> lock(mutex_);
	components_[entity] &= ~ChunkMask(chunkFlag);
}

bool ChunkStatusManager::HasStatus(const Entity entity, ChunkFlag chunkFlag)
{
	if (entity >= components_.size())
	{
		std::ostringstream oss;
		oss << "[Error] Accessing entity: " << entity << " while chunk array is of size: " <<
			components_.size();
		LogDebug(oss.str());
		return false;
	}
	return (components_[entity] & ChunkMask(chunkFlag)) == ChunkMask(chunkFlag);
}

std::vector<Index> ChunkStatusManager::GetAccessibleChunks()
{
	std::lock_guard<std::mutex> lock(mutex_);
	std::vector<Index> accessibleChunks;
	for (size_t index = 0; index < components_.size(); index++)
	{
		if (!HasStatus(index, ChunkFlag::LOADED) ||
			!HasStatus(index, ChunkFlag::ACCESSIBLE) ||
			HasStatus(index, ChunkFlag::EMPTY) ||
			HasStatus(index, ChunkFlag::OCCLUDED))
			continue;
			accessibleChunks.push_back(index);
		
	}
	return accessibleChunks;
}

std::vector<Index> ChunkStatusManager::GetRenderedChunks()
{
	std::lock_guard<std::mutex> lock(mutex_);
	std::vector<Index> renderedChunks;
	for (size_t index = 0; index < components_.size(); index++)
	{
		if (!HasStatus(index, ChunkFlag::LOADED) ||
			!HasStatus(index, ChunkFlag::VISIBLE) ||
			HasStatus(index, ChunkFlag::EMPTY) ||
			HasStatus(index, ChunkFlag::OCCLUDED))
			continue;
		renderedChunks.push_back(index);
	}
	return
		renderedChunks;
}

std::vector<Index> ChunkStatusManager::GetDirtyChunks()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("Chunks_System::SetChunkOcclusionCulling::GetLoadedChunks",
		profiler::colors::Pink700);
#endif
	std::lock_guard<std::mutex> lock(mutex_);
	std::vector<Index> dirtyChunks;
	for (size_t index = 0; index < components_.size(); index++)
	{
		if (HasStatus(index, ChunkFlag::DIRTY))
		{
			dirtyChunks.push_back(index);
		}
	}
	return dirtyChunks;
}

std::vector<Index> ChunkStatusManager::GetLoadedChunks()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("Chunks_System::SetChunkOcclusionCulling::GetLoadedChunks",
	           profiler::colors::Pink700);
#endif
	std::lock_guard<std::mutex> lock(mutex_);
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
	const Vec3f chunkPos = Vec3f(components_[chunkIndex]) * kChunkSize + Vec3f(kChunkSize / 2.0f);
	return {chunkPos, Vec3f(kChunkSize / 2.0f)};
}

//-----------------------------------------------------------------------------
// ChunkRenderManager
//-----------------------------------------------------------------------------
ChunkRenderManager::ChunkRenderManager(
	EntityManager& entityManager,
	ChunkContentManager& chunkContentManager)
	: ComponentManager<ChunkRender, ComponentType::CHUNK_RENDER>(entityManager),
	  chunkContentManager_(chunkContentManager)
{
	entityManager.RegisterComponentManager(*this);
	ResizeIfNecessary(components_, INIT_ENTITY_NMB - 1, ChunkRender{});
}

Index ChunkRenderManager::AddComponent(const Entity chunkIndex)
{
	std::lock_guard<std::mutex> lock(mutex_);
	ResizeIfNecessary(components_, chunkIndex, ChunkRender{});
	entityManager_.AddComponentType(chunkIndex,
	                                static_cast<EntityMask>(ComponentType::CHUNK_RENDER));

	return chunkIndex;
}

void ChunkRenderManager::Init(const Entity chunkIndex)
{
	std::lock_guard<std::mutex> lock(mutex_);
	components_[chunkIndex].cube.Init();
	glGenBuffers(1, &components_[chunkIndex].vbo);
	glCheckError();
}

void ChunkRenderManager::SetChunkValues(
	const Entity chunkIndex,
	ChunkContentVector chunkContentVector)
{
	const auto blocks = chunkContentVector.blocks;
	glCheckError();
	if (blocks.empty()) return;

	std::lock_guard<std::mutex> lock(mutex_);
	glBindVertexArray(components_[chunkIndex].cube.vao);
	glBindBuffer(GL_ARRAY_BUFFER, components_[chunkIndex].vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ChunkContent) * blocks.size(), &blocks[0], GL_STATIC_DRAW);
	glCheckError();
	glVertexAttribIPointer(5,
	                       1,
	                       GL_SHORT,
	                       sizeof(ChunkContent),
	                       (void*)offsetof(ChunkContent, blockId));
	glVertexAttribDivisor(5, 1);
	glEnableVertexAttribArray(5);
	glCheckError();
	glVertexAttribIPointer(6,
	                       1,
	                       GL_UNSIGNED_INT,
	                       sizeof(ChunkContent),
	                       (void*)offsetof(ChunkContent, texId));
	glVertexAttribDivisor(6, 1);
	glEnableVertexAttribArray(6);
	glCheckError();
}

void ChunkRenderManager::SetChunkValues(
	const Entity chunkIndex,
	std::vector<ChunkContent> chunkContentVector)
{
	const auto blocks = chunkContentVector;
	glCheckError();
	if (blocks.empty()) return;

	std::lock_guard<std::mutex> lock(mutex_);
	glBindVertexArray(components_[chunkIndex].cube.vao);
	glBindBuffer(GL_ARRAY_BUFFER, components_[chunkIndex].vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ChunkContent) * blocks.size(), &blocks[0], GL_STATIC_DRAW);
	glCheckError();
	glVertexAttribIPointer(5,
	                       1,
	                       GL_SHORT,
	                       sizeof(ChunkContent),
	                       (void*)offsetof(ChunkContent, blockId));
	glVertexAttribDivisor(5, 1);
	glEnableVertexAttribArray(5);
	glCheckError();
	glVertexAttribIPointer(6,
	                       1,
	                       GL_UNSIGNED_INT,
	                       sizeof(ChunkContent),
	                       (void*)offsetof(ChunkContent, texId));
	glVertexAttribDivisor(6, 1);
	glEnableVertexAttribArray(6);
	glCheckError();
}

void ChunkRenderManager::Draw(const Entity chunkIndex)
{
	std::lock_guard<std::mutex> lock(mutex_);
	components_[chunkIndex].cube.DrawInstanced(chunkContentManager_.GetChunkSize(chunkIndex));
}

void ChunkRenderManager::DestroyComponent(const Entity chunkIndex)
{
	std::lock_guard<std::mutex> lock(mutex_);
	components_[chunkIndex].cube.Destroy();
	glDeleteBuffers(1, &components_[chunkIndex].vbo);
	entityManager_.RemoveComponentType(chunkIndex,
	                                   static_cast<EntityMask>(ComponentType::CHUNK_RENDER));
}

//-----------------------------------------------------------------------------
// ChunkManager
//-----------------------------------------------------------------------------
ChunkManager::ChunkManager(EntityManager& entityManager)
	: chunkContentManager(entityManager),
	  chunkStatusManager(entityManager, chunkContentManager),
	  chunkPosManager(entityManager),
	  chunkRenderManager(entityManager, chunkContentManager)
{
}

void ChunkManager::AddComponent(const Entity chunkIndex)
{
	chunkContentManager.AddComponent(chunkIndex);
	chunkStatusManager.AddComponent(chunkIndex);
	chunkPosManager.AddComponent(chunkIndex);
	chunkRenderManager.AddComponent(chunkIndex);
}

void ChunkManager::DestroyComponent(const Entity chunkIndex)
{
	chunkContentManager.DestroyComponent(chunkIndex);
	chunkStatusManager.DestroyComponent(chunkIndex);
	chunkPosManager.DestroyComponent(chunkIndex);
	chunkRenderManager.DestroyComponent(chunkIndex);
}

//-----------------------------------------------------------------------------
// ChunksViewer
//-----------------------------------------------------------------------------
ChunkViewer::ChunkViewer(
	EntityManager& entityManager,
	ChunkManager& chunkManager)
	: entityManager_(entityManager),
	  chunkManager_(chunkManager)
{
}

void ChunkViewer::DrawImGui(const Entity selectedEntity) const
{
	using namespace ImGui;
	if (selectedEntity == INVALID_ENTITY)
		return;

	if (TreeNode("Chunk"))
	{
		if (entityManager_.HasComponent(selectedEntity,
		                                static_cast<EntityMask>(ComponentType::CHUNK_POS)))
		{
			auto chunkPos = Vec3i(chunkManager_.chunkPosManager.GetComponent(selectedEntity));
			DragInt3("Chunk Index", &chunkPos[0]);
		}
		if (entityManager_.HasComponent(selectedEntity,
		                                static_cast<EntityMask>(ComponentType::CHUNK_STATUS)))
		{
			std::string status = "";
			if (chunkManager_.chunkStatusManager.HasStatus(selectedEntity, ChunkFlag::EMPTY))
				status.append("EMPTY ");
			if (chunkManager_.chunkStatusManager.HasStatus(selectedEntity, ChunkFlag::ACCESSIBLE))
				status.append("ACCESSIBLE ");
			if (chunkManager_.chunkStatusManager.HasStatus(selectedEntity, ChunkFlag::VISIBLE))
				status.append("VISIBLE ");
			if (chunkManager_.chunkStatusManager.HasStatus(selectedEntity, ChunkFlag::LOADED))
				status.append("LOADED ");

			Text("Status: %s", status.c_str());
		}
		if (entityManager_.HasComponent(selectedEntity,
		                                static_cast<EntityMask>(ComponentType::CHUNK_CONTENT)))
		{
			if (TreeNode("Chunk Content"))
			{
				const auto blocks = chunkManager_.chunkContentManager.GetBlocks(selectedEntity);
				for (auto& block : blocks)
				{
					const BlockTex blockTex = TexHashToBlockTex(block.texId);
					if (TreeNode(("ID " + std::to_string(block.blockId)).c_str()))
					{
						Text("Side Tex ID: %u", blockTex.sideTexId);
						Text("Top Tex ID: %u", blockTex.topTexId);
						Text("Bottom Tex ID: %u", blockTex.bottomTexId);
						TreePop();
					}
				}
				TreePop();
			}
		}
		TreePop();
	}
}
}
