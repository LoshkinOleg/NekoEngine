#pragma once
#include <utility>

namespace neko
{
const static size_t kMaxUiElements = 16;

struct UiElement
{
	Vec3f position = Vec3f::zero; //In percent
	Vec2u size = Vec2u(100u); //In pixel

	std::string texturePath = "";
	TextureId textureId = INVALID_TEXTURE_ID;

	gl::RenderQuad quad{Vec3f::zero, Vec2f::one};

	explicit UiElement(const Vec3f& pos = Vec3f::zero,
	                   const Vec2u& newSize = Vec2u::one)
		: position(pos), size(newSize) {}
	
	explicit UiElement(std::string texPath, 
	                   const Vec3f& pos = Vec3f::zero,
	                   const Vec2u& newSize = Vec2u::one)
		: position(pos), size(newSize), texturePath(std::move(texPath)) {}
	
	void Destroy()
	{
		quad.Destroy();
		gl::DestroyTexture(textureId);
	}
};

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
                             public IUiManager
{
public:
	UiManager() = default;

	void Init() override;

	void Update(seconds dt) override {}
	void FixedUpdate() override {}

	void AddUiElement(UiElement* uiElement) override;

	void Render() override;

	void Destroy() override;

private:
	std::mutex updateMutex_;

	gl::Shader uiShader_;
	std::vector<UiElement*> uiElements_{};
};

using BlockManagerLocator = Locator<IBlockManager, NullBlockManager>;
}