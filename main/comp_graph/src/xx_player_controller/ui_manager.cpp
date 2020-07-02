#include <xx_player_controller/ui_manager.h>

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

void UiManager::Init()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	const auto& config = BasicEngine::GetInstance()->config;
	uiShader_.LoadFromFile(
		config.dataRootPath + "shaders/base_ui.vert",
		config.dataRootPath + "shaders/base.frag");

	uiElements_.reserve(kMaxUiElements);
}

void UiManager::AddUiElement(UiElement* uiElement)
{
	uiElements_.push_back(uiElement);

	if (uiElement->textureId == INVALID_TEXTURE_ID)
		uiElement->textureId = stbCreateTexture(uiElement->texturePath, gl::Texture::CLAMP_WRAP);
	
	const auto& config = BasicEngine::GetInstance()->config;
	uiElement->Init(config.windowSize);
}

void UiManager::Render()
{
	const auto& config = BasicEngine::GetInstance()->config;
	
	uiShader_.Bind();
	uiShader_.SetInt("tex", 0);
	glDisable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);
	for (auto& element : uiElements_)
	{
		element->Draw(config.windowSize);
	}
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
}

void UiManager::Destroy()
{
	for (auto& element : uiElements_)
	{
		element->Destroy();
		delete element;
	}
	uiElements_.clear();
}
}
