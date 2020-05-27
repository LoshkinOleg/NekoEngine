#include "chunks_manager.h"

#include "chunk_renderer.h"

namespace neko
{
ChunksManager::ChunksManager(EntityManager& entityManager)
	: ComponentManager<Chunk, ComponentType::CHUNK>(entityManager)
{
}

neko::Index neko::ChunksManager::AddComponent(Entity entity)
{
	components_[entity].init = true;
	return entity;
}

void ChunksManager::DestroyComponent(Entity entity)
{
	components_[entity].init = false;
}

void ChunksManager::SetComponent(Entity entity, const Chunk& component)
{
	components_[entity] = component;
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
			if (ImGui::Checkbox("Init", &chunk.init))
			{
				chunksManager_.SetComponent(selectedEntity, chunk);
			}
		}
	}
}

}
