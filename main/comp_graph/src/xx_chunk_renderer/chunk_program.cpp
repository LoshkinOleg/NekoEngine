#include "xx_chunk_renderer/chunk_program.h"


#include "minelib/aabb_manager.h"
#include "stb_image.h"

namespace neko
{
HelloChunkRenderer::HelloChunkRenderer()
	: gizmosRenderer_(camera_),
	  entityManager_(engine_.entityManager),
	  blockManager_(engine_.blockManager),
	  aabbManager_(engine_.componentsManagerSystem.aabbManager),
	  chunkContentManager_(engine_.componentsManagerSystem.chunkContentManager),
	  chunkRenderManager_(engine_.componentsManagerSystem.chunkRenderManager),
	  chunkPosManager_(engine_.componentsManagerSystem.chunkPosManager),
	  chunkStatusManager_(engine_.componentsManagerSystem.chunkStatusManager),
	  chunkRenderer_(engine_, camera_)
{
}

void HelloChunkRenderer::Init()
{
	stbi_set_flip_vertically_on_load(true);
	camera_.position = Vec3f(kChunkSize * kRenderDist + kChunkSize / 2.0f, 1.0f, kChunkSize * kRenderDist + kChunkSize / 2.0f);
	camera_.LookAt(Vec3f::one);
		
	blockManager_.Init();
	gizmosRenderer_.Init();
	for (auto& chunk : chunks_)
	{
		chunk = entityManager_.CreateEntity();
		chunkContentManager_.AddComponent(chunk);
		chunkPosManager_.AddComponent(chunk);
		chunkRenderManager_.AddComponent(chunk);
		chunkStatusManager_.AddComponent(chunk);
		chunkStatusManager_.AddStatus(chunk, ChunkFlag::VISIBLE);
		for (uint16_t id = 0; id < kChunkBlockCount; ++id)
		{
			chunkContentManager_.SetBlock(chunk, blockManager_.GetBlock(rand() % 6 + 1), id);
		}
	
		chunkRenderManager_.SetChunkValues(chunk);
	}
	
	for (uint8_t x = 0; x < kChunkNumDiam; ++x)
	{
		for (uint8_t z = 0; z < kChunkNumDiam; ++z)
		{
			chunkPosManager_.SetComponent(x + z * kChunkNumDiam, Vec3i(x, -1, z));
		}
	}

	chunkRenderer_.Init();
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void HelloChunkRenderer::Update(seconds dt)
{
	camera_.Update(dt);
}

void HelloChunkRenderer::Render()
{
	chunkRenderer_.Render();
	
	Ray rayOut;
	const Vec3f direction = camera_.reverseDirection.Normalized() * -1;
	float minDist = std::numeric_limits<float>::max();
	Entity closestChunk = INVALID_ENTITY;
	for (auto& chunk : chunks_)
	{
		const Vec3f& pos = Vec3f(chunkPosManager_.GetComponent(chunk) * kChunkSize) + Vec3f(kChunkSize / 2.0f);
		const float dist = (pos - camera_.position).SquareMagnitude();
		if (dist < minDist)
		{
			minDist = dist;
			closestChunk = chunk;
		}
	}

	if (aabbManager_.RaycastBlockInChunk(rayOut, camera_.position, direction, closestChunk))
	{
		glLineWidth(3.0f);
		gizmosRenderer_.DrawCube(rayOut.hitAabb.CalculateCenter(), Vec3f(kCubeHalfSize + 0.01f) * 2, Color::black);
		gizmosRenderer_.Render();
	}
}

void HelloChunkRenderer::Destroy()
{
	for (auto& chunk : chunks_)
	{
		entityManager_.DestroyEntity(chunk);
	}
	blockManager_.Destroy();
	gizmosRenderer_.Destroy();
}

void HelloChunkRenderer::DrawImGui()
{
}

void HelloChunkRenderer::OnEvent(const SDL_Event& event)
{
	camera_.OnEvent(event);
}
}
