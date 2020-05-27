#pragma once
#include <engine/transform.h>
#include <sdl_engine/sdl_camera.h>

#include "chunk_renderer.h"

namespace neko
{
    class DrawSystem final : public SystemInterface, public sdl::SdlEventSystemInterface, public DrawImGuiInterface
    {
    public:
	    explicit DrawSystem(MinecraftLikeEngine& engine);
        void Init() override;
        void DrawImGui() override;

        void Update(seconds dt) override;
        void Destroy() override;
        void OnEvent(const SDL_Event& event) override;
    protected:
        std::mutex updateMutex_;
        ChunkRenderer chunkRenderer_;
    	
        //ImGui Viewer
        EntityViewer entityViewer_;
        Transform3dViewer transformViewer_;
        MinecraftLikeEngine& engine_;
        MoveableCamera3D camera_;
    };
}
