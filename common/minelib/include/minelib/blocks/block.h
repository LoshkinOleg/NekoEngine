#pragma once
#include "gl/shape.h"

namespace neko
{
const static float kCubeHalfSize = 0.5f;

using TextureHash = uint32_t;
using TextureId8 = uint8_t;
struct alignas(sizeof(TextureHash)) BlockTex
{
	explicit BlockTex(const uint8_t sideId = 0,
	                  const uint8_t topId = 0,
	                  const uint8_t bottomId = 0)
		: sideTexId(sideId), topTexId(topId), bottomTexId(bottomId)
	{}
	
	TextureId8 sideTexId = 0;
	TextureId8 topTexId = 0;
	TextureId8 bottomTexId = 0;
};

struct Block final
{
	explicit Block(const char* blockName,
	               const TextureId8 sideId = 0,
	               const TextureId8 topId = 0,
	               const TextureId8 bottomId = 0) : name(blockName)
	{
		blockTex.sideTexId = sideId;
		blockTex.topTexId = topId;
		blockTex.bottomTexId = bottomId;
	}
	
	const char* name = "";
	size_t id = -1;

	TextureId previewTexture = 0;

	BlockTex blockTex;
};

static TextureHash BlockTexToBlockHash(const BlockTex& blockTex)
{
    TextureHash result = blockTex.sideTexId;
    result = (result << 8) + blockTex.topTexId;
    result = (result << 8) + blockTex.bottomTexId;
	return result;
}

static BlockTex BlockHashToBlockTex(const TextureHash blockHash)
{
	return BlockTex(blockHash >> 16 & 0xFF, blockHash >> 8 & 0xFF, blockHash & 0xFF);
}
}
