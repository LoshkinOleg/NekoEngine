#pragma once
#include <engine/system.h>


namespace neko
{
class EntityManager;
class Transform3dManager;
class ChunksManager;
class MinecraftLikeEngine;

class ChunksSystem final : public SystemInterface
{
public:
	explicit ChunksSystem(MinecraftLikeEngine& engine);

	void GenerateChunks();
	
	void Init() override;

	void Update(seconds dt) override;

	void Destroy() override;

private:
	ChunksManager& chunksManager_;
	Transform3dManager& transform3dManager_;
	EntityManager& entityManager_;
};
}
