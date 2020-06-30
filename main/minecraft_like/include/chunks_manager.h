#pragma once
#include <engine/component.h>

#include "chunk.h"

namespace neko
{
class ChunksManager final : public neko::ComponentManager<Chunk, ComponentType::CHUNK>
{
	using ComponentManager::ComponentManager;
public :
	void AddVisibleChunk(Entity chunkIndex);
	void RemoveVisibleChunk(Entity chunkIndex);
	void ClearVisibleChunks();
	void ReserveVisibleChunks(const float size);
	void AddLoadedChunk(Entity chunkIndex);
	std::vector<Entity> GetVisibleChunks();
	std::vector<Entity> GetLoadedChunks();
	

private:
	std::vector<Entity> loadedChunks_;
	std::vector<Entity> visibleChunks_;
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
