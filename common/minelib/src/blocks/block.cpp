#include "minelib/blocks/block.h"

#include "stb_image.h"

#include "gl/texture.h"

namespace neko
{
void Block::Init()
{
	stbi_set_flip_vertically_on_load(true);

	cube.Init();
	textureId = gl::stbCreateTexture(texturePath);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Block::Draw() const
{
	glBindTexture(GL_TEXTURE_2D, textureId);
	cube.Draw();
}

void Block::Destroy()
{
	cube.Destroy();
	gl::DestroyTexture(textureId);
	gl::DestroyTexture(previewTexture);
}

void UniqueBlock::Init()
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

void UniqueBlock::Draw() const
{
	cube.Draw();
}

void UniqueBlock::Destroy()
{
	cube.Destroy();
	gl::DestroyTexture(previewTexture);
}
}
