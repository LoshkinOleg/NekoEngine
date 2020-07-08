#pragma once
#include <array>

#include "gl/shape.h"

namespace neko
{
struct Block final
{
	explicit Block(const char* blockName,
	               const uint8_t sideId = 0,
	               const uint8_t topId = 0,
	               const uint8_t bottomId = 0)
		: name(blockName), sideTexId(sideId), topTexId(topId), bottomTexId(bottomId)
	{
	}
	
	const char* name = "";
	size_t id = -1;

	TextureId previewTexture = 0;

	uint8_t sideTexId = 0;
	uint8_t topTexId = 0;
	uint8_t bottomTexId = 0;
};
}
