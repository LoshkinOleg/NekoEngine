#include "xx_chunk_renderer/chunk_program.h"


#include "minelib/aabb_manager.h"
#include "stb_image.h"

namespace neko
{
HelloChunkRenderer::HelloChunkRenderer()
	: gizmosRenderer_(Camera3D()),
	  playerController_(engine_),
	  entityManager_(engine_.entityManager),
	  blockManager_(engine_.blockManager),
	  uiManager_(engine_.uiManager),
	  aabbManager_(engine_.componentsManagerSystem.aabbManager),
	  chunkManager_(engine_.componentsManagerSystem.chunkManager),
	  playerManager_(engine_.componentsManagerSystem.playerManager),
	  chunkRenderer_(engine_, Camera3D())
{
}

void HelloChunkRenderer::Init()
{
	stbi_set_flip_vertically_on_load(true);

	const Entity newPlayer = entityManager_.CreateEntity();
	playerController_.SetCurrentPlayer(newPlayer);
	playerManager_.AddComponent(newPlayer);
	playerController_.SetCurrentPlayer(newPlayer);

	Player& curPlayer = playerManager_.GetComponent(newPlayer);
	gizmosRenderer_.SetCamera(curPlayer.camera);
	chunkRenderer_.SetCamera(curPlayer.camera);
	
	curPlayer.position = Vec3f(kChunkSize * kRenderDist + kChunkSize / 2.0f, 17.0f, kChunkSize * kRenderDist + kChunkSize / 2.0f);
	curPlayer.camera.position = curPlayer.position;
	curPlayer.camera.WorldLookAt(Vec3f::one);
		
	blockManager_.Init();
	glCheckError();
	gizmosRenderer_.Init();
	glCheckError();
	for (auto& chunk : chunks_)
	{
		chunk = entityManager_.CreateEntity();
		chunkManager_.AddComponent(chunk);
		chunkManager_.chunkPosManager.SetComponent(chunk, 
			Vec3i(chunk % kChunkNumDiam, chunk / kChunkNumDiam % kChunkNumDiam, chunk / (kChunkNumDiam * kChunkNumDiam)));
		chunkManager_.chunkStatusManager.AddStatus(chunk, ChunkFlag::LOADED);
	}
	
	for (uint16_t x = 0; x < kChunkNumDiam; ++x)
	{
		for (uint16_t z = 0; z < kChunkNumDiam; ++z)
		{
			const Entity chunk = chunks_[x + z * kChunkNumDiam * kChunkNumDiam];
			for (uint16_t id = 0; id < kChunkBlockCount; ++id)
			{
				chunkManager_.chunkContentManager.SetBlock(chunk, id, blockManager_.GetRandomBlock());
			}
			chunkManager_.chunkRenderManager.Init(chunk);
			chunkManager_.chunkRenderManager.SetChunkValues(chunk);
		}
	}

	glCheckError();
	chunkRenderer_.Init();
	glCheckError();
	playerController_.Init();
	uiManager_.Init();
	glCheckError();
	
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

	curPlayer.camera.Init();
	playerManager_.SetComponent(newPlayer, curPlayer);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void HelloChunkRenderer::Update(const seconds dt)
{
	//Display Chunks Gizmos
	const auto loadedChunks = chunkManager_.chunkStatusManager.GetLoadedChunks();
	for (auto loadedChunk : loadedChunks)
	{
		if (entityManager_.HasComponent(loadedChunk,
		                                static_cast<EntityMask>(ComponentType::CHUNK_POS)))
		{
			gizmosRenderer_.DrawCube(
				Vec3f(chunkManager_.chunkPosManager.GetComponent(loadedChunk) * kChunkSize) + Vec3f(
					(kChunkSize - 1) / 2.0f),
				Vec3f::one * kChunkSize,
				playerManager_.GetCurrentChunk(playerController_.GetCurrentPlayer()) == loadedChunk
					? Color::yellow
					: Color::blue, 2.0f);
		}
	}
	
	playerController_.Update(dt);
}

void HelloChunkRenderer::FixedUpdate()
{
	playerController_.FixedUpdate();
}

void HelloChunkRenderer::Render()
{
	chunkRenderer_.Render();
	
	gizmosRenderer_.Render();

	const Player curPlayer = playerManager_.GetComponent(playerController_.GetCurrentPlayer());
	skyboxCube_.Draw(curPlayer.camera.GenerateViewMatrix(), curPlayer.camera.GenerateProjectionMatrix());
	
	uiManager_.Render();
}

void HelloChunkRenderer::Destroy()
{
	for (auto& chunk : chunks_)
	{
		entityManager_.DestroyEntity(chunk);
	}
	blockManager_.Destroy();
	gizmosRenderer_.Destroy();
	uiManager_.Destroy();
	
	glDeleteTextures(1, &skyboxTexture_);
	skyboxCube_.Destroy();
	skyboxShader_.Destroy();
}

void HelloChunkRenderer::DrawImGui()
{
}

void HelloChunkRenderer::OnEvent(const SDL_Event& event)
{
	playerController_.OnEvent(event);
}
}
