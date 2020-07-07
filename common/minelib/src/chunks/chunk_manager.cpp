#include "minelib/chunks/chunk_manager.h"

#include "minelib/chunks/chunk_renderer.h"

namespace neko
{

	void ChunkStatutManager::AddStatut(Entity entity, ChunkFlag chunkFlag)
	{
		components_[entity] |= ChunkMask(chunkFlag);
	}

	void ChunkStatutManager::RemoveStatut(Entity entity, ChunkFlag chunkFlag)
	{
		components_[entity] &= ~ChunkMask(chunkFlag);
	}

	bool ChunkStatutManager::HasStatut(Entity entity, ChunkFlag chunkFlag) const
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

	std::vector<Index> ChunkStatutManager::GetAccessibleChunks() const
	{
		std::vector<Index> accessibleChunks;
		for (size_t index = 0; index < components_.size(); index++)
		{
			if (HasStatut(index, ChunkFlag::ACCESSIBLE))
			{
				accessibleChunks.push_back(index);
			}
		}
		return accessibleChunks;
	}

	std::vector<Index> ChunkStatutManager::GetVisibleChunks() const
	{
		std::vector<Index> accessibleChunks;
		for (size_t index = 0; index < components_.size(); index++)
		{
			if (HasStatut(index, ChunkFlag::VISIBLE))
			{
				accessibleChunks.push_back(index);
			}
		}
		return accessibleChunks;
	}

	std::vector<Index> ChunkStatutManager::GetLoadedChunks() const
	{
		std::vector<Index> accessibleChunks;
		for (size_t index = 0; index < components_.size(); index++)
		{
			if (HasStatut(index, ChunkFlag::LOADED))
			{
				accessibleChunks.push_back(index);
			}
		}
		return accessibleChunks;
	}

	Aabb3d ChunkPosManager::GetAabb(const Entity chunkIndex) const
{
	Aabb3d aabb;
	Vec3i chunkPos = components_[chunkIndex];
	aabb.SetFromCenter(Vec3f(chunkPos * kChunkSize),
	                   Vec3f(kChunkSize / 2, kChunkSize / 2, kChunkSize / 2));
	return aabb;
}

void ChunkContentManager::SetBlock(const Entity chunkIndex, const uint8_t blockId, const Vec3i& pos)
{
	components_[chunkIndex][pos.x + pos.y * kChunkSize + pos.z * kChunkSize * kChunkSize] = blockId;
}

uint8_t ChunkContentManager::GetBlockId(const Entity chunkIndex, const Vec3i& pos) const
{
	return components_[chunkIndex][pos.x + pos.y * kChunkSize + pos.z * kChunkSize * kChunkSize];
}

ChunksViewer::ChunksViewer(
	EntityManager& entityManager,
	ChunkContentManager& chunkContentManager,
	ChunkStatutManager& chunkStatutManager,
	ChunkPosManager& chunkPosManager)
	: entityManager_(entityManager),
	  chunkContentManager_(chunkContentManager),
	  chunkStatutManager_(chunkStatutManager),
	  chunkPosManager_(chunkPosManager)
{
}

void ChunksViewer::DrawImGui(const Entity selectedEntity)
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

