#pragma once
#include "player_engine.h"

#include "graphics/camera.h"
#include "mathematics/aabb.h"

#include "minelib/blocks/block_manager.h"
#include "minelib/minecraft_like_engine.h"

namespace neko
{
class PlayerEngine;

class PlayerProgram final : public RenderProgram, public sdl::SdlEventSystemInterface, public DrawImGuiInterface
{
public:
    explicit PlayerProgram(PlayerEngine& engine);
	
    void Init() override;

    void Update(seconds dt) override;
	void FixedUpdate() override;

    void Destroy() override;

    void Render() override;

    void DrawImGui() override;

    void OnEvent(const SDL_Event& event) override;
private:
    MoveableCamera3D camera_;

	PlayerEngine& testEngine_;
	
	MinecraftLikeEngine engine_;
	BlockManager& blockManager_;
	Transform3dManager& transform3dManager_;
	ChunkManager& chunkManager_;
	ChunkSystem& chunkSystem_;
	ChunkRenderer chunkRenderer_;
	GizmosRenderer gizmosRenderer_;

	Entity currentChunk_ = INVALID_ENTITY;
};
}
