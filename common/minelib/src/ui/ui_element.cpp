#include "minelib/ui/ui_element.h"

namespace neko
{
void UiElement::Init(const Vec2u& screenSize)
{
	const Vec2f normalSpaceSize = Vec2f(size) / Vec2f(screenSize);
	quad = gl::RenderQuad(position, normalSpaceSize);
	quad.Init();
}

void UiElement::Draw(const Vec2u& screenSize)
{
	glBindTexture(GL_TEXTURE_2D, textureId);
	const Vec2f normalSpaceSize = Vec2f(size) / Vec2f(screenSize);
	quad.SetValues(normalSpaceSize, position);
	quad.Draw();
}

void UiElement::Destroy()
{
	quad.Destroy();
	gl::DestroyTexture(textureId);
}
}
