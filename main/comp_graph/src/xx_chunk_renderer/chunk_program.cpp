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
	  transform3dManager_(engine_.componentsManagerSystem.transform3dManager),
	  chunkManager_(engine_.componentsManagerSystem.chunkManager),
	  chunkRenderer_(engine_, camera_)
{
}

void HelloChunkRenderer::Init()
{
	stbi_set_flip_vertically_on_load(true);
	camera_.position = Vec3f(kChunkSize * kRenderDist + kChunkSize / 2.0f, 17.0f, kChunkSize * kRenderDist + kChunkSize / 2.0f);
	camera_.LookAt(Vec3f::one);
		
	blockManager_.Init();
	gizmosRenderer_.Init();
	for (auto& chunk : chunks_)
	{
		chunk = entityManager_.CreateEntity();
		chunkManager_.AddComponent(chunk);
		chunkManager_.chunkPosManager.SetComponent(chunk, 
			Vec3i(chunk % kChunkNumDiam, chunk / kChunkNumDiam % kChunkNumDiam, chunk / (kChunkNumDiam * kChunkNumDiam)));
		transform3dManager_.AddComponent(chunk);
		transform3dManager_.SetPosition(chunk, 
			Vec3f(chunkManager_.chunkPosManager.GetComponent(chunk) * kChunkSize) + Vec3f(kChunkSize / 2.0f));
		chunkManager_.chunkStatusManager.AddStatus(chunk, ChunkFlag::LOADED);
	}
	
	for (uint16_t x = 0; x < kChunkNumDiam; ++x)
	{
		for (uint16_t z = 0; z < kChunkNumDiam; ++z)
		{
			for (uint16_t id = 0; id < kChunkBlockCount; ++id)
			{
				chunkManager_.chunkContentManager.SetBlock(chunks_[x + z * kChunkNumDiam * kChunkNumDiam], blockManager_.GetBlock(rand() % 6 + 1), id);
			}
			chunkManager_.chunkRenderManager.SetChunkValues(chunks_[x + z * kChunkNumDiam * kChunkNumDiam]);
		}
	}

	chunkRenderer_.Init();
	
	stbi_set_flip_vertically_on_load(false);
	const auto& config = BasicEngine::GetInstance()->config;
	skyboxCube_.Init();
	skyboxTexture_ = gl::LoadCubemap({
		config.dataRootPath + "sprites/skybox_bluesky/right.png",
		config.dataRootPath + "sprites/skybox_bluesky/left.png",
		config.dataRootPath + "sprites/skybox_bluesky/top.png",
		config.dataRootPath + "sprites/skybox_bluesky/bottom.png",
		config.dataRootPath + "sprites/skybox_bluesky/front.png",
		config.dataRootPath + "sprites/skybox_bluesky/back.png"
	});
	skyboxCube_.SetTexture(skyboxTexture_);
	
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
	
	Entity closestChunk = INVALID_ENTITY;
	for (auto& chunk : chunks_)
	{
		if (!chunkManager_.chunkStatusManager.HasStatus(chunk, ChunkFlag::LOADED))
			continue;
		
		if (chunkManager_.chunkPosManager.GetAabb(chunk).IntersectAabb(Aabb3d(camera_.position, Vec3f(0.2f))))
		{
			closestChunk = chunk;
			break;
		}
	}

	std::vector<Entity> chunksInFront;
	chunksInFront.push_back(closestChunk);
	for (auto& chunk : chunks_)
	{
		if (!chunkManager_.chunkStatusManager.HasStatus(chunk, ChunkFlag::LOADED) ||
			chunkManager_.chunkStatusManager.HasStatus(chunk, ChunkFlag::EMPTY))
			continue;

		const auto& chunkPos = chunkManager_.chunkPosManager.GetComponent(chunk);
		if (camera_.reverseDirection.x < 0)
		{
			if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::right)
				chunksInFront.push_back(chunk);
			
			if (camera_.reverseDirection.y < 0)
			{
				if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::right + Vec3i::up)
					chunksInFront.push_back(chunk);
			}
			else
			{
				if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::right + Vec3i::down)
					chunksInFront.push_back(chunk);
			}
			
			if (camera_.reverseDirection.z < 0)
			{
				if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::right + Vec3i::forward)
					chunksInFront.push_back(chunk);
			}
			else
			{
				if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::right + Vec3i::back)
					chunksInFront.push_back(chunk);
			}
		}
		else
		{
			if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::left)
				chunksInFront.push_back(chunk);
			
			if (camera_.reverseDirection.y < 0)
			{
				if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::left + Vec3i::up)
					chunksInFront.push_back(chunk);
			}
			else
			{
				if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::left + Vec3i::down)
					chunksInFront.push_back(chunk);
			}
			
			if (camera_.reverseDirection.z < 0)
			{
				if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::left + Vec3i::forward)
					chunksInFront.push_back(chunk);
			}
			else
			{
				if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::left + Vec3i::back)
					chunksInFront.push_back(chunk);
			}
		}
		
		if (camera_.reverseDirection.y < 0)
		{
			if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::up)
				chunksInFront.push_back(chunk);
		}
		else
		{
			if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::down)
				chunksInFront.push_back(chunk);
		}
		
		if (camera_.reverseDirection.z < 0)
		{
			if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::forward)
				chunksInFront.push_back(chunk);
			
			if (camera_.reverseDirection.x < 0)
			{
				if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::forward + Vec3i::right)
					chunksInFront.push_back(chunk);
			}
			else
			{
				if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::forward + Vec3i::left)
					chunksInFront.push_back(chunk);
			}
			
			if (camera_.reverseDirection.y < 0)
			{
				if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::forward + Vec3i::up)
					chunksInFront.push_back(chunk);
			
				if (camera_.reverseDirection.x < 0)
				{
					if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::forward + Vec3i::up + Vec3i::right)
						chunksInFront.push_back(chunk);
				}
				else
				{
					if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::forward + Vec3i::up + Vec3i::left)
						chunksInFront.push_back(chunk);
				}
			}
			else
			{
				if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::forward + Vec3i::down)
					chunksInFront.push_back(chunk);
			
				if (camera_.reverseDirection.x < 0)
				{
					if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::forward + Vec3i::down + Vec3i::right)
						chunksInFront.push_back(chunk);
				}
				else
				{
					if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::forward + Vec3i::down + Vec3i::left)
						chunksInFront.push_back(chunk);
				}
			}
		}
		else
		{
			if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::back)
				chunksInFront.push_back(chunk);
			
			if (camera_.reverseDirection.x < 0)
			{
				if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::back + Vec3i::right)
					chunksInFront.push_back(chunk);
			}
			else
			{
				if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::back + Vec3i::left)
					chunksInFront.push_back(chunk);
			}
			
			if (camera_.reverseDirection.y < 0)
			{
				if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::back + Vec3i::up)
					chunksInFront.push_back(chunk);
			
				if (camera_.reverseDirection.x < 0)
				{
					if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::back + Vec3i::up + Vec3i::right)
						chunksInFront.push_back(chunk);
				}
				else
				{
					if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::back + Vec3i::up + Vec3i::left)
						chunksInFront.push_back(chunk);
				}
			}
			else
			{
				if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::back + Vec3i::down)
					chunksInFront.push_back(chunk);
			
				if (camera_.reverseDirection.x < 0)
				{
					if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::back + Vec3i::down + Vec3i::right)
						chunksInFront.push_back(chunk);
				}
				else
				{
					if (chunkPos == chunkManager_.chunkPosManager.GetComponent(closestChunk) + Vec3i::back + Vec3i::down + Vec3i::left)
						chunksInFront.push_back(chunk);
				}
			}
		}
	}

	for (size_t i = 1; i < chunksInFront.size(); ++i)
	{
		const auto& chunkPos = transform3dManager_.GetPosition(chunksInFront[i]);
		for (size_t j = 1; j < chunksInFront.size(); ++j)
		{
			if (chunksInFront[j] == chunksInFront[i]) continue;
			
			const auto& otherChunkPos = transform3dManager_.GetPosition(chunksInFront[j]);
			if ((chunkPos - camera_.position).SquareMagnitude() < (otherChunkPos - camera_.position).SquareMagnitude())
				std::swap(chunksInFront[i], chunksInFront[j]);
		}
	}
	
	Ray rayOut;
	const Vec3f direction = camera_.reverseDirection.Normalized() * -1;
	for (auto& chunkInFront : chunksInFront)
	{
		if (aabbManager_.RaycastBlockInChunk(rayOut, camera_.position, direction, chunkInFront))
		{
			if (rayOut.hitDist > maxReach_) break;
			
			glLineWidth(3.0f);
			gizmosRenderer_.DrawCube(rayOut.hitAabb.CalculateCenter(), Vec3f(kCubeHalfSize + 0.01f) * 2, Color::black);
			gizmosRenderer_.Render();
			break;
		}
	}
	
	skyboxCube_.Draw(camera_.GenerateViewMatrix(), camera_.GenerateProjectionMatrix());
}

void HelloChunkRenderer::Destroy()
{
	for (auto& chunk : chunks_)
	{
		entityManager_.DestroyEntity(chunk);
	}
	blockManager_.Destroy();
	gizmosRenderer_.Destroy();
	
	glDeleteTextures(1, &skyboxTexture_);
	skyboxCube_.Destroy();
	skyboxShader_.Destroy();
}

void HelloChunkRenderer::DrawImGui()
{
}

void HelloChunkRenderer::OnEvent(const SDL_Event& event)
{
	camera_.OnEvent(event);
}
}
