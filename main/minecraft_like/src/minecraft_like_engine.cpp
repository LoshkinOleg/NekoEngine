#include <minecraft_like_engine.h>

namespace neko
{
	MinecraftLikeEngine::MinecraftLikeEngine(Configuration* config) : SdlEngine(config), chunkManager(entityManager), chunksSystem_(*this){

		RegisterSystem(drawSystem_);
		RegisterSystem(chunksSystem_);
		
	}

    void MinecraftLikeEngine::Init() {
		SdlEngine::Init();
	}

    void MinecraftLikeEngine::Destroy() {
		drawSystem_.Destroy();
		SdlEngine::Destroy();
	    
	}

    void MinecraftLikeEngine::ManageEvent() {
		SdlEngine::ManageEvent();
	    
	}

    void MinecraftLikeEngine::GenerateUiFrame() {
		SdlEngine::GenerateUiFrame();
	    
	}
}
