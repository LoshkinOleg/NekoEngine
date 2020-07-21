#include "minelib/ui/ui_element.h"

namespace neko
{
void UiElement::Init(const Vec2u& screenSize)
{
	const Vec2f normalSpaceSize = Vec2f(size) / Vec2f(screenSize);
	quad = gl::RenderQuad(position, normalSpaceSize);
	quad.Init();
}

void UiElement::Draw(const Vec2u& screenSize) const
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	quad.Draw();
}

void UiElement::Update(const Vec2u& screenSize)
{
	const Vec2f normalSpaceSize = Vec2f(size) / Vec2f(screenSize);
	quad.SetValues(normalSpaceSize, position);
}

void UiElement::Destroy()
{
	quad.Destroy();
	gl::DestroyTexture(textureId);
}
}
