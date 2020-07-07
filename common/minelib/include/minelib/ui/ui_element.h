#pragma once
#include <utility>

#include "gl/texture.h"

namespace neko
{
struct UiElement
{
	enum UiFlags : uint8_t
	{
		RESIZE = 1u << 0u,
	};
	
	Vec3f position = Vec3f::zero; //In percent
	Vec2u size = Vec2u(100u); //In pixel

	std::string texturePath = "";
	TextureId textureId = INVALID_TEXTURE_ID;

	uint8_t flags = 0;

	gl::RenderQuad quad{Vec3f::zero, Vec2f::one};

	explicit UiElement(const Vec3f& pos = Vec3f::zero,
	                   const Vec2u& newSize = Vec2u::one)
		: position(pos), size(newSize) {}
	
	explicit UiElement(std::string texPath, 
	                   const Vec3f& pos = Vec3f::zero,
	                   const Vec2u& newSize = Vec2u::one)
		: position(pos), size(newSize), texturePath(std::move(texPath)) {}

	void Init(const Vec2u& screenSize);
	
	void Draw(const Vec2u& screenSize);
	void Update(const Vec2u& screenSize);

	void Destroy();
};
}