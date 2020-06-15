#pragma once
#include <engine/component.h>

#include "chunk.h"

namespace neko
{
class ChunksManager final : public neko::ComponentManager<Chunk, ComponentType::CHUNK>
{
public:
	explicit ChunksManager(EntityManager& entityManager);
private:
};

class ChunksViewer
{
public:
	explicit ChunksViewer(EntityManager& entityManager, ChunksManager& chunksManager);
	void DrawImGui(Entity selectedEntity);
protected:
	EntityManager& entityManager_;
	ChunksManager& chunksManager_;
};
}
