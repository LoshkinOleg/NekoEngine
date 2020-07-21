#include "minelib/ui/ui_manager.h"

namespace neko
{
UiManager::UiManager()
{
}

void UiManager::Init()
{
	//RendererLocator::get().Render(this);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const auto& config = BasicEngine::GetInstance()->config;
	uiShader_.LoadFromFile(
		config.dataRootPath + "shaders/base_ui.vert",
		config.dataRootPath + "shaders/base.frag");

	uiElements_.reserve(kMaxUiElements);
}

void UiManager::Update(seconds dt)
{
	//RendererLocator::get().Render(this);
}

void UiManager::AddUiElement(UiElement* uiElement)
{

	if (uiElement->textureId == INVALID_TEXTURE_ID)
		uiElement->textureId = gl::stbCreateTexture(uiElement->texturePath, gl::Texture::CLAMP_WRAP);
	
	const auto& config = BasicEngine::GetInstance()->config;
	uiElement->Init(config.windowSize);
	uiElements_.push_back(uiElement);
}

void UiManager::Render()
{
	uiShader_.Bind();
	uiShader_.SetInt("tex", 0);
	glDisable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);
	
	const auto& config = BasicEngine::GetInstance()->config;
	for (auto& element : uiElements_)
	{
		if (element->flags & UiElement::DIRTY)
		{
			element->Update(config.windowSize);
			element->flags &= ~UiElement::DIRTY;
		}
		element->Draw(config.windowSize);
	}
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
}

void UiManager::OnEvent(const SDL_Event& event)
{
	if(event.window.event == SDL_WINDOWEVENT_RESIZED)
	{
		for (auto& element : uiElements_)
		{
			element->flags |= UiElement::DIRTY;
		}
	}
}

void UiManager::Destroy()
{
	uiElements_.clear();
}
}
