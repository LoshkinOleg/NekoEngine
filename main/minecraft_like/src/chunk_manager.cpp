#include "chunks_manager.h"

#include "chunk_renderer.h"

namespace neko
{
void ChunksManager::AddVisibleChunk(Entity chunkIndex)
{
	visibleChunks_.push_back(chunkIndex);
}

void ChunksManager::RemoveVisibleChunk(Entity chunkIndex)
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

void ChunksManager::AddLoadedChunk(Entity chunkIndex)
{
	loadedChunks_.push_back(chunkIndex);
}

std::vector<Entity> ChunksManager::GetVisibleChunks()
{
	return visibleChunks_;
}

std::vector<Entity> ChunksManager::GetLoadedChunks()
{
	return loadedChunks_;
}

ChunksViewer::ChunksViewer(EntityManager& entityManager, ChunksManager& chunksManager) :
	entityManager_(entityManager),
	chunksManager_(chunksManager)
{
}

void ChunksViewer::DrawImGui(Entity selectedEntity)
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