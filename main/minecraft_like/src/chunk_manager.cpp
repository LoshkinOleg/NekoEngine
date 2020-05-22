//
// Created by efarhan on 26.01.20.
//

#include "chunk_renderer.h"
#include "chunks_manager.h"

namespace neko
{
	ChunksManager::ChunksManager(EntityManager& entityManager)
		: ComponentManager<Chunk, ComponentType::OTHER_TYPE>(entityManager) {
	}

	neko::Index neko::ChunksManager::AddComponent(Entity entity) {
		components_[entity].init = true;
		return entity;
	}

    void ChunksManager::DestroyComponent(Entity entity) {
		components_[entity].init = false;
	    
	}
    void ChunksManager::SetComponent(Entity entity, const Chunk& component) {
	    
	}
}
