#pragma once
#include "minelib/ui/ui_element.h"

namespace neko
{
const static size_t kMaxUiElements = 16;

//-----------------------------------------------------------------------------
// UiManagerInterface
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
class IUiManager
{
public:
	virtual void AddUiElement(UiElement* uiElement) = 0;
protected:
	~IUiManager() = default;
};

//-----------------------------------------------------------------------------
// NullUiManager
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
class NullUiManager final : public IUiManager
{
	void AddUiElement([[maybe_unused]] UiElement* uiElement) override {}
};

//-----------------------------------------------------------------------------
// UiManager
//-----------------------------------------------------------------------------
/// \brief Draw gizmos
class UiManager final : public RenderCommandInterface,
                        public SystemInterface,
                        public sdl::SdlEventSystemInterface,
                        public IUiManager
{
public:
	UiManager() = default;

	void Init() override;

	void Update(seconds dt) override {}
	void FixedUpdate() override {}

	void AddUiElement(UiElement* uiElement) override;

	void Render() override;

	void OnEvent(const SDL_Event& event) override;

	void Destroy() override;

private:
	std::mutex updateMutex_;

	gl::Shader uiShader_;
	std::vector<UiElement*> uiElements_{};
};

using UiManagerLocator = Locator<IUiManager, NullUiManager>;
}