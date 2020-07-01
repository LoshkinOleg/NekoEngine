#pragma once
#include <engine/component.h>

#include "chunk.h"

namespace neko
{
class ChunksManager final : public neko::ComponentManager<Chunk, ComponentType::CHUNK>
{
	using ComponentManager::ComponentManager;
public :
	void AddVisibleChunk(const Entity chunkIndex);

	void RemoveVisibleChunk(const Entity chunkIndex);

	void ClearVisibleChunks();

	void ReserveVisibleChunks(const float size);

	void AddLoadedChunk(const Entity chunkIndex);

	std::vector<Entity> GetVisibleChunks() const;

	std::vector<Entity> GetLoadedChunks() const;

private:
	std::vector<Entity> loadedChunks_;
	std::vector<Entity> visibleChunks_;
};

class ChunksViewer
{
public:
	explicit ChunksViewer(EntityManager& entityManager, ChunksManager& chunksManager);

	void DrawImGui(const Entity selectedEntity);

protected:
	EntityManager& entityManager_;
	ChunksManager& chunksManager_;
};
}
