#pragma once

#include <sdl_engine/sdl_engine.h>

namespace neko::minecraftlike
{
	class MinecraftLikeEngine : public sdl::SdlEngine
	{
	public:
		explicit MinecraftLikeEngine(Configuration* config = nullptr);
	};
}