#pragma once

#include "chunk_renderer.h"

namespace neko
{
    class DrawSystem : public SystemInterface, public sdl::SdlEventSystemInterface
    {
    public:
        void Init() override;

        void Update(seconds dt) override;
        void Destroy() override;
    	void OnEvent(const SDL_Event& event) override;
    protected:

        ChunkRenderer chunkRenderer_;
    };
}