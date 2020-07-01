#include "chunk_renderer.h"

#include <graphics/camera.h>
#ifdef EASY_PROFILE_USE
#include <easy/profiler.h>
#endif

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

		glEnable(GL_DEPTH_TEST);
	}

	void ChunkRenderer::Update(seconds dt)
	{
		std::lock_guard<std::mutex> lock(updateMutex_);
	}

	void ChunkRenderer::Render()
	{
#ifdef EASY_PROFILE_USE
		EASY_BLOCK("ChunkRenderer::Render");
#endif
		if (shader_.GetProgram() == 0) return;

		std::lock_guard<std::mutex> lock(updateMutex_);

		shader_.Bind();
		shader_.SetMat4("view", camera_.GenerateViewMatrix());
		shader_.SetMat4("projection", camera_.GenerateProjectionMatrix());

		frustum_.ConstructFrustum(camera_.position, -camera_.reverseDirection, camera_.nearPlane, camera_.farPlane, camera_.GetFovX(), camera_.fovY, camera_.GetUp(), camera_.GetRight()); //TODO do this on camera movement instead of here

		for (size_t i = 0; i < INIT_ENTITY_NMB; i++)
		{
			if (!engine_.entityManager_.HasComponent(i, static_cast<EntityMask>(ComponentType::CHUNK))) { continue; }
			Chunk chunk = engine_.componentsManagerSystem_.chunkManager_.GetComponent(i);
			if (frustum_.ContainsCube(chunk.GetAabb()))
			{
#ifdef EASY_PROFILE_USE
				EASY_BLOCK("ChunkRenderer::Render::Chunk");
#endif
				for (int x = 0; x < kChunkSize; x++)
				{
					for (int y = 0; y < kChunkSize; y++)
					{
						for (int z = 0; z < kChunkSize; z++)
						{
							if (frustum_.ContainsSphere(Vec3f(x, y, z), 2)) //TODO replace by if contains block's aabb
							{
#ifdef EASY_PROFILE_USE
								EASY_BLOCK("ChunkRenderer::Render::Air");
#endif
								int blockID = chunk.GetBlockId(Vec3i(x, y, z));
								if (blockID != 0)
								{
									if (texture_[blockID - 1] == INVALID_TEXTURE_ID) continue;
#ifdef EASY_PROFILE_USE
									EASY_BLOCK("ChunkRenderer::Render::Cube");
#endif
									Mat4f model = Mat4f::Identity; //model transform matrix
									model = Transform3d::Translate(model, Vec3f(x, y, z) + chunk.GetChunkPos());
									shader_.SetMat4("model", model);
									glBindTexture(GL_TEXTURE_2D, texture_[blockID - 1]); //bind texture id to texture slot
									glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
									glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

									cube_.Draw();
								}
							}
						}
					}
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