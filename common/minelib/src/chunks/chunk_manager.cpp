#include "minelib/chunks/chunk_manager.h"
#include "minelib/chunks/chunk_renderer.h"

namespace neko
{
ChunksManager::ChunksManager(EntityManager& entityManager)
	: ComponentManager<Chunk, ComponentType::CHUNK>(entityManager)
{
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

			for (int x = 0; x < kChunkSize; x++)
			{
				for (int y = 0; y < kChunkSize / 2; y++)
				{
					for (int z = 0; z < kChunkSize; z++)
					{
						int id = chunk.GetBlockId(Vec3i(x, y, z));
						std::string text = "Pos " + std::to_string(x) + ", " + std::to_string(y) +	", " + std::to_string(z) + " Id : " + std::to_string(id);
						if (ImGui::Selectable(text.c_str(), false))
						{
						}
					}
				}
			}
		}
	}
}

}