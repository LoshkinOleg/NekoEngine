#include <gl/gles3_window.h>
#include <gl/graphics.h>

#include "minecraft_like_engine.h"

int main(int argc, char** argv)
{
	neko::sdl::Gles3Window window;
	neko::gl::Gles3Renderer renderer;
	neko::MinecraftLikeEngine engine;

	engine.SetWindowAndRenderer(&window, &renderer);
	engine.Init();
	engine.EngineLoop();
	return 0;
}
