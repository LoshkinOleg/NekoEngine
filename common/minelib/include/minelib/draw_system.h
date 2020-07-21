#pragma once
#include <sdl_engine/sdl_camera.h>

#include "minelib/aabb_manager.h"
#include "minelib/chunks/chunk_renderer.h"
#include "minelib/chunks/chunk_manager.h"

#include "gizmos_renderer.h"
#include "player/player_controller.h"

namespace neko
{
class DrawSystem final : public SystemInterface,
                         public sdl::SdlEventSystemInterface,
                         public DrawImGuiInterface
{
public:
    explicit DrawSystem(MinecraftLikeEngine& engine);
    void Init() override;
    void DrawImGui() override;

    void Update(seconds dt) override;
	void FixedUpdate() override {}
    void Destroy() override;
    void OnEvent(const SDL_Event& event) override;
	
protected:
    FpsCamera* camera_;
    MinecraftLikeEngine& engine_;
	
    ChunkRenderer chunkRenderer_;
    ChunkSystem& chunkSystem_;
    GizmosRenderer gizmosRenderer_;
	PlayerController playerController_;
	PlayerManager& playerManager_;
	
    //ImGui Viewer
    EntityViewer entityViewer_;
    ChunkViewer chunkViewer_;
};
}
