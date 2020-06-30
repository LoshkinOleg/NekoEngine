#pragma once
#include <array>

#include <stb_image.h>

#include <gl/shape.h>

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

	void Init() override
	{
		stbi_set_flip_vertically_on_load(true);
		
		cube.Init();
		textureId = gl::stbCreateTexture(texturePath);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	void Draw() const override
	{
		glBindTexture(GL_TEXTURE_2D, textureId);
		cube.Draw();
	}

	void Destroy() override
	{
		cube.Destroy();
		gl::DestroyTexture(textureId);
		gl::DestroyTexture(previewTexture);
	}

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

	void Init() override
	{
		cube.Init();
		
		std::array<TextureId, 3> cubeTex{};
		cubeTex[0] = gl::stbCreateTexture(texturePaths[0]);
		glBindTexture(GL_TEXTURE_2D, cubeTex[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		cubeTex[1] = gl::stbCreateTexture(texturePaths[1]);
		glBindTexture(GL_TEXTURE_2D, cubeTex[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		cubeTex[2] = gl::stbCreateTexture(texturePaths[2]);
		glBindTexture(GL_TEXTURE_2D, cubeTex[2]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		cube.SetTextures(cubeTex);
	}

	void Draw() const override
	{
		cube.Draw();
	}

	void Destroy() override
	{
		cube.Destroy();
		gl::DestroyTexture(previewTexture);
	}

	std::array<std::string, 3> texturePaths{""};
	
	gl::RenderCuboidUnique cube{Vec3f::zero, Vec3f::one};
};
}
