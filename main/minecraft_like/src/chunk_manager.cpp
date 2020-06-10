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
	for (int x = 0; x < chunkSize; x++)
	{
		for (int y = 0; y < chunkSize/2; y++)
		{
			for (int z = 0; z < chunkSize; z++)
			{
				if (y < chunkSize/2 -2)
				{
					if (RandomRange(0.0f, 1.0f) < 0.75f)
					{
						components_[entity].SetBlock(2, Vec3i(x, y, z));
					} else
					{
						components_[entity].SetBlock(3, Vec3i(x, y, z));
					}
				} else
				{
					components_[entity].SetBlock(1, Vec3i(x, y, z));
				}
			}
		}
	}
	return entity;
}

void ChunksManager::DestroyComponent(Entity entity)
{
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

			for (int x = 0; x < chunkSize; x++)
			{
				for (int y = 0; y < chunkSize / 2; y++)
				{
					for (int z = 0; z < chunkSize; z++)
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
