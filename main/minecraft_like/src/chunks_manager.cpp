#include "chunks_manager.h"

#include "imgui.h"

namespace neko
{
void ChunksManager::AddVisibleChunk(const Entity chunkIndex)
{
	visibleChunks_.push_back(chunkIndex);
}

void ChunksManager::RemoveVisibleChunk(const Entity chunkIndex)
{
	visibleChunks_.erase(std::remove(visibleChunks_.begin(), visibleChunks_.end(), chunkIndex));
}

void ChunksManager::ClearVisibleChunks()
{
	visibleChunks_.clear();
}

void ChunksManager::ReserveVisibleChunks(const float size)
{
	visibleChunks_.reserve(size);
}

void ChunksManager::AddLoadedChunk(const Entity chunkIndex)
{
	loadedChunks_.push_back(chunkIndex);
}

std::vector<Entity> ChunksManager::GetVisibleChunks() const
{
	return visibleChunks_;
}

std::vector<Entity> ChunksManager::GetLoadedChunks() const
{
	return loadedChunks_;
}

ChunksViewer::ChunksViewer(EntityManager& entityManager, ChunksManager& chunksManager)
	: entityManager_(entityManager),
	  chunksManager_(chunksManager)
{
}

void ChunksViewer::DrawImGui(const Entity selectedEntity)
{
	if (selectedEntity == INVALID_ENTITY)
		return;
	if (entityManager_.HasComponent(selectedEntity, static_cast<EntityMask>(ComponentType::CHUNK)))
	{
		if (ImGui::CollapsingHeader("Chunk"))
		{
			Chunk chunk = chunksManager_.GetComponent(selectedEntity);
			bool visible = chunk.IsVisible();
			ImGui::Checkbox("Visible", &visible);
			auto chunkPos = chunk.GetChunkPos();
			ImGui::DragInt3("ChunkPos", &chunkPos[0]);
		}
	}
}
}
