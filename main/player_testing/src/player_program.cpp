#include "player_program.h"

namespace neko
{
PlayerProgram::PlayerProgram(PlayerEngine& engine)
	: testEngine_(engine),
	  blockManager_(engine_.blockManager),
	  transform3dManager_(engine_.componentsManagerSystem.transform3dManager),
	  chunkManager_(engine_.componentsManagerSystem.chunkManager),
	  chunkSystem_(engine_.componentsManagerSystem.chunkSystem),
	  chunkRenderer_(engine_, camera_),
	  gizmosRenderer_(camera_)
{
	testEngine_.RegisterSystem(camera_);
	testEngine_.RegisterOnEvent(camera_);

	testEngine_.RegisterSystem(blockManager_);
	testEngine_.RegisterSystem(chunkSystem_);
	testEngine_.RegisterSystem(chunkRenderer_);
	testEngine_.RegisterSystem(gizmosRenderer_);
}

void PlayerProgram::Init()
{
	camera_.position = Vec3f(8.0f) * 10.0f;
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void PlayerProgram::Update(const seconds dt)
{
}

void PlayerProgram::FixedUpdate()
{
	const Vec3i currentChunkPos = 
		Vec3i(
			std::floor(camera_.position.x / kChunkSize),
			std::floor(camera_.position.y / kChunkSize),
			std::floor(camera_.position.z / kChunkSize)
		);

	if (currentChunk_ == INVALID_ENTITY ||
		currentChunkPos != chunkManager_.chunkPosManager.GetComponent(currentChunk_))
	{
		const auto visibleChunks = chunkManager_.chunkStatusManager.GetVisibleChunks();
		for (auto& visibleChunk : visibleChunks)
		{
			if (chunkManager_.chunkPosManager.GetComponent(visibleChunk) == currentChunkPos)
			{
				currentChunk_ = visibleChunk;
				break;
			}
		}
	}
}

void PlayerProgram::Render()
{
	if (currentChunk_ != INVALID_ENTITY)
	{
		gizmosRenderer_.DrawCube(
			transform3dManager_.GetPosition(currentChunk_), Vec3f::one * kChunkSize, Color::yellow);
	}
}

void PlayerProgram::Destroy()
{
	engine_.Destroy();
}

void PlayerProgram::DrawImGui()
{
}

void PlayerProgram::OnEvent(const SDL_Event& event)
{
}
}
