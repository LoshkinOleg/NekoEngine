#pragma once
#include "minelib/aabb_manager.h"
#include "minelib/chunks/chunk_manager.h"
#include "minelib/chunks/chunk_system.h"
#include "player/player_manager.h"

namespace neko
{
class ComponentsManagerSystem final : public SystemInterface
{
public:
	explicit ComponentsManagerSystem(MinecraftLikeEngine& engine);

	void Init() override;

	void Update(seconds dt) override;
	
	void FixedUpdate() override {}

	void Destroy() override;

	AabbManager aabbManager;
	ChunkSystem chunkSystem;
	ChunkManager chunkManager;
	PlayerManager playerManager;
};
}
