#include "chunks_renderer.h"

#ifdef EASY_PROFILE_USE
#include <easy/profiler.h>
#endif
#include <gl/texture.h>

#include <graphics/camera.h>
#include <engine/transform.h>

#include "minecraft_like_engine.h"
#include "chunks_manager.h"

namespace neko
{
ChunksRenderer::ChunksRenderer(
	MinecraftLikeEngine& engine,
	MoveableCamera3D& camera,
	EntityViewer& entityViewer)
	: camera_(camera),
	  engine_(engine),
	  entityViewer_(entityViewer),
	  entityManager_(engine.entityManager_),
	  chunksManager_(engine.componentsManagerSystem_.chunksManager_),
	  transform3dManager_(engine.componentsManagerSystem_.transform3dManager_)
{
}

void ChunksRenderer::Init()
{
	const auto& config = BasicEngine::GetInstance()->config;
	shader_.LoadFromFile(
		config.dataRootPath + "shaders/minecraft_like/coords.vert",
		config.dataRootPath + "shaders/minecraft_like/coords.frag");
	texture_[0] = gl::stbCreateTexture(config.dataRootPath + "sprites/blocks/dirt.jpg");
	texture_[1] = gl::stbCreateTexture(config.dataRootPath + "sprites/blocks/stone.jpg");
	texture_[2] = gl::stbCreateTexture(config.dataRootPath + "sprites/blocks/diamond_ore.jpg");
	cube_.Init();

	glEnable(GL_DEPTH_TEST);
}

void ChunksRenderer::Update(seconds dt)
{
	std::lock_guard<std::mutex> lock(updateMutex_);
	transform3dManager_.SetPosition(0, camera_.position);
}

void ChunksRenderer::Render()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("ChunksRenderer::Render");
#endif
	if (shader_.GetProgram() == 0) return;

	std::lock_guard<std::mutex> lock(updateMutex_);

	shader_.Bind();
	shader_.SetMat4("view", camera_.GenerateViewMatrix());
	shader_.SetMat4("projection", camera_.GenerateProjectionMatrix());
	const auto loadedChunks = chunksManager_.GetLoadedChunks();
	for (auto loadedChunk : loadedChunks)
	{
		const Chunk chunk = chunksManager_.GetComponent(loadedChunk);
		if (!chunk.IsVisible()) continue;
		const Vec3f chunkPos = transform3dManager_.GetPosition(
			loadedChunk);
		for (unsigned x = 0; x < kChunkSize; x++)
		{
			for (unsigned y = 0; y < kChunkSize; y++)
			{
				for (unsigned z = 0; z < kChunkSize; z++)
				{
					const int blockId = chunk.GetBlockId(Vec3i(x, y, z));
					if (blockId != 0)
					{
						if (texture_[blockId - 1] == INVALID_TEXTURE_ID) continue;
						Mat4f model = Mat4f::Identity; //model transform matrix
						model = Transform3d::Translate(model, Vec3f(x, y, z) + chunkPos);
						shader_.SetMat4("model", model);
						glBindTexture(GL_TEXTURE_2D, texture_[blockId - 1]);
						//bind texture id to texture slot
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

						cube_.Draw();
					}
				}
			}
		}
	}
}

void ChunksRenderer::Destroy()
{
	cube_.Destroy();

	shader_.Destroy();
	gl::DestroyTexture(texture_[0]);
	gl::DestroyTexture(texture_[1]);
	gl::DestroyTexture(texture_[2]);
}
}
