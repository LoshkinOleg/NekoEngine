#include "chunk_renderer.h"

#include <graphics/camera.h>


#include "gl/texture.h"
#include "minecraft_like_engine.h"

namespace neko
{
ChunkRenderer::ChunkRenderer(
	MinecraftLikeEngine& engine,
	MoveableCamera3D& camera,
	EntityViewer& entityViewer)
	: camera_(camera),
	engine_(engine),
	entityViewer_(entityViewer)
{
}

void ChunkRenderer::Init()
{
	const auto& config = BasicEngine::GetInstance()->config;
	shader_.LoadFromFile(
		config.dataRootPath + "shaders/minecraft_like/coords.vert",
		config.dataRootPath + "shaders/minecraft_like/coords.frag");
	texture_[0] = gl::stbCreateTexture(config.dataRootPath + "sprites/blocks/dirt.jpg");
	texture_[1] = gl::stbCreateTexture(config.dataRootPath + "sprites/blocks/stone.jpg");
	texture_[2] = gl::stbCreateTexture(config.dataRootPath + "sprites/blocks/diamond_ore.jpg");
	cube_.Init();
	camera_.position = Vec3f::forward * 3;

	glEnable(GL_DEPTH_TEST);
}

void ChunkRenderer::Update(seconds dt)
{
	std::lock_guard<std::mutex> lock(updateMutex_);
}

void ChunkRenderer::Render()
{
	if (shader_.GetProgram() == 0) return;

	std::lock_guard<std::mutex> lock(updateMutex_);

	shader_.Bind();
	shader_.SetMat4("view", camera_.GenerateViewMatrix());
	shader_.SetMat4("projection", camera_.GenerateProjectionMatrix());
	for (size_t i = 0; i < INIT_ENTITY_NMB; i++)
	{
		if (!engine_.entityManager_.HasComponent(i, static_cast<EntityMask>(ComponentType::CHUNK))) { continue; }
		Chunk chunk = engine_.componentsManagerSystem_.chunkManager_.GetComponent(i);
		Vec3f chunkPos = engine_.componentsManagerSystem_.transform3dManager_.GetPosition(i);

		for (int i = 0; i < kChunkSize * kChunkSize * kChunkSize; i++)
		{
			int z = std::floor(i / (kChunkSize * kChunkSize));
			int y = std::floor((i - z * kChunkSize * kChunkSize) / kChunkSize);
			int x = i % kChunkSize;
			Mat4f model = Mat4f::Identity; //model transform matrix
			model = Transform3d::Translate(model, Vec3f(x, y, z) + chunkPos);
			shader_.SetMat4("model", model);
			int blockID = chunk.GetBlockId(Vec3i(x, y, z));
			if (texture_[blockID - 1] == INVALID_TEXTURE_ID) return;
			glBindTexture(GL_TEXTURE_2D, texture_[blockID - 1]); //bind texture id to texture slot
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			if (blockID != 0)
			{
				cube_.Draw();
			}
		}
	}
}

void ChunkRenderer::Destroy()
{
	cube_.Destroy();

	shader_.Destroy();
	gl::DestroyTexture(texture_[0]);
	gl::DestroyTexture(texture_[1]);
	gl::DestroyTexture(texture_[2]);
}

}