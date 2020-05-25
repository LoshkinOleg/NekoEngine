#pragma once

#include "chunk_renderer.h"

namespace neko
{
    class DrawSystem : public SystemInterface
    {
    public:
        void Init() override;

        void Update(seconds dt) override;
        void Destroy() override;
    protected:

        ChunkRenderer chunkRenderer_;
    };
}