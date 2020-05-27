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
}
}
