//
// Created by efarhan on 25.01.20.
//
#include <gl/graphics.h>
#include "gl/gles3_window.h"
#include "player_engine.h"


int main(int argc, char** argv)
{
    neko::sdl::Gles3Window window;
    neko::gl::Gles3Renderer renderer;
    neko::PlayerEngine engine;

    engine.SetWindowAndRenderer(&window, &renderer);

    engine.Init();
    engine.EngineLoop();
    return 0;
}