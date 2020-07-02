#pragma once
#include <array>

#include <gl/shape.h>
#include <gl/texture.h>

namespace neko
{
struct BaseBlock : RenderableObject
{
	const char* name = "";
	size_t id = -1;

	TextureId previewTexture;

	void Init() override {}
	void Draw() const override {}
	void Destroy() override {}
};

struct Block final : BaseBlock
{
	Block(const char* blockName,
	      const std::string_view& blockTexturePath)
		: texturePath(blockTexturePath)
	{
		name = blockName;
	}

	void Init() override;
	void Draw() const override;
	void Destroy() override;

	std::string texturePath = "";
	TextureId textureId = 0;

	gl::RenderCuboid cube{Vec3f::zero, Vec3f::one};
};

struct UniqueBlock final : BaseBlock
{
	UniqueBlock(const char* blockName,
	            std::array<std::string, 3> blockTexturePaths)
		: texturePaths(std::move(blockTexturePaths))
	{
		name = blockName;
	}

	void Init() override;
	void Draw() const override;
	void Destroy() override;

	std::array<std::string, 3> texturePaths{""};
	
	gl::RenderCuboidUnique cube{Vec3f::zero, Vec3f::one};
};
}
