#pragma once
#include "chunks_manager.h"
#include "engine/system.h"

namespace neko
{
class MinecraftLikeEngine;

class ChunksSystem : public SystemInterface
    {
    public:
        explicit ChunksSystem(MinecraftLikeEngine& engine);
        void Init() override;

        void Update(seconds dt) override;
        void Destroy() override;
    private:
        ChunksManager& chunksManager_;
    };
}
