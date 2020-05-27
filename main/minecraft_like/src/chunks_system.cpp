#include "chunks_system.h"

#include <iostream>

#include "minecraft_like_engine.h"
#include "chunks_manager.h"

namespace neko
{
ChunksSystem::ChunksSystem(MinecraftLikeEngine& engine)
	: chunksManager_(engine.componentsManagerSystem_.chunkManager_)
{
	Chunk chunk;
	chunksManager_.SetComponent(0, chunk);
}

void ChunksSystem::Init()
{
}

void ChunksSystem::Update(seconds dt)
{
	//for (Index i = 0; i < 100; i++)
	//{
	//	std::cout << chunksManager_.GetComponent(i).init;
	//}
	//std::cout << std::endl;
}

void ChunksSystem::Destroy()
{
}
}