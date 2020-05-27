#include "chunks_system.h"
#include "minecraft_like_engine.h"

#include <iostream>

namespace neko
{
ChunksSystem::ChunksSystem(MinecraftLikeEngine& engine)
	: chunksManager_(engine.chunkManager)
{
	Chunk chunk;
	chunk.init = true;
	chunksManager_.SetComponent(0, chunk);
}

void ChunksSystem::Init()
{
}

void ChunksSystem::Update(seconds dt)
{
	for (Index i = 0; i < 100; i++)
	{
	}
}

void ChunksSystem::Destroy()
{
}
}
