//
// Created by efarhan on 26.01.20.
//

#include "draw_system.h"

#include <imgui.h>

namespace neko
{
void DrawSystem::Init()
{
	chunkRenderer_.Init();
}

void DrawSystem::Update(seconds dt)
{
	chunkRenderer_.Update(dt);
	RendererLocator::get().Render(&chunkRenderer_);
}

void DrawSystem::Destroy()
{
	chunkRenderer_.Destroy();
}

void DrawSystem::OnEvent(const SDL_Event& event)
{
	chunkRenderer_.OnEvent(event);
}
}
