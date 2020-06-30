#include <xx_player_controller/ui_manager.h>

namespace neko
{
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
	const Vec2f normalSpaceSize = Vec2f(uiElement->size) / Vec2f(config.windowSize);
	uiElement->quad = gl::RenderQuad(uiElement->position, normalSpaceSize);
	uiElement->quad.Init();
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
		glBindTexture(GL_TEXTURE_2D, element->textureId);
		const Vec2f normalSpaceSize = Vec2f(element->size) / Vec2f(config.windowSize);
		element->quad.SetValues(normalSpaceSize, element->position);
		element->quad.Draw();
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
