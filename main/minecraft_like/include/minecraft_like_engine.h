#pragma once

#include <sdl_engine/sdl_engine.h>

#include "chunks_system.h"
#include "draw_system.h"
#include "engine/scene.h"

namespace neko
{
	class MinecraftLikeEngine : public sdl::SdlEngine
	{
	public:
		MinecraftLikeEngine(Configuration* config = nullptr);

        void Init() override;
        void Destroy() override;
        void ManageEvent() override;
        void GenerateUiFrame() override;
		EntityManager entityManager;
		ChunksManager chunkManager;

	private :
		DrawSystem drawSystem_;
		ChunksSystem chunksSystem_;
	};
}
