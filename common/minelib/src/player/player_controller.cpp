#include "minelib/player/player_controller.h"

#include "minelib/blocks/block.h"
#include "minelib/chunks/chunk.h"

namespace neko
{
PlayerController::PlayerController(MinecraftLikeEngine& engine)
	: engine_(engine),
	  entityManager_(engine.entityManager),
	  blockManager_(engine.blockManager),
	  playerManager_(engine.componentsManagerSystem.playerManager),
	  chunkManager_(engine.componentsManagerSystem.chunkManager),
	  inputManager_(sdl::InputLocator::get()),
	  gizmosRenderer_(GizmosLocator::get()),
	  aabbManager_(AabbLocator::get()),
	  uiManager_(UiManagerLocator::get())
{
}

void PlayerController::Init()
{
}

void PlayerController::Update(const seconds dt)
{
	const auto players = entityManager_.FilterEntities(static_cast<EntityMask>(ComponentType::PLAYER));
	for (auto& player : players)
	{
		Player curPlayer = playerManager_.GetComponent(player);
		curPlayer.camera.Update(dt);
		
		const Vec3i currentChunkPos = Vec3i(
			std::floor(curPlayer.position.x / kChunkSize),
			std::floor(curPlayer.position.y / kChunkSize),
			std::floor(curPlayer.position.z / kChunkSize)
		);
		if (curPlayer.currentChunk == INVALID_ENTITY || 
			currentChunkPos != chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk))
		{
			const auto chunks = chunkManager_.chunkStatusManager.GetVisibleChunks();
			for (auto& chunk : chunks)
			{
				if (!chunkManager_.chunkStatusManager.HasStatus(chunk, ChunkFlag::LOADED))
					continue;

				if (chunkManager_.chunkPosManager.GetAabb(chunk).IntersectAabb(Aabb3d(curPlayer.position, Vec3f(0.2f))))
				{
					curPlayer.currentChunk = chunk;
					break;
				}
			}
		}

		playerManager_.SetComponent(player, curPlayer);
	}

	if (currentPlayer_ != INVALID_ENTITY)
	{
		Player curPlayer = playerManager_.GetComponent(currentPlayer_);
		if (curPlayer.currentChunk != INVALID_ENTITY)
		{
			//Gets the chunks in front
			std::vector<Entity> chunksInFront{curPlayer.currentChunk};
			const auto chunks = chunkManager_.chunkStatusManager.GetVisibleChunks();
			for (auto& chunk : chunks)
			{
				if (!chunkManager_.chunkStatusManager.HasStatus(chunk, ChunkFlag::LOADED) ||
					chunkManager_.chunkStatusManager.HasStatus(chunk, ChunkFlag::EMPTY))
					continue;

				const auto& chunkPos = chunkManager_.chunkPosManager.GetComponent(chunk);
				if (curPlayer.camera.reverseDirection.x < 0)
				{
					if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::right)
						chunksInFront.push_back(chunk);
					
					if (curPlayer.camera.reverseDirection.y < 0)
					{
						if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::right + Vec3i::up)
							chunksInFront.push_back(chunk);
					}
					else
					{
						if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::right + Vec3i::down)
							chunksInFront.push_back(chunk);
					}
					
					if (curPlayer.camera.reverseDirection.z < 0)
					{
						if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::right + Vec3i::forward)
							chunksInFront.push_back(chunk);
					}
					else
					{
						if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::right + Vec3i::back)
							chunksInFront.push_back(chunk);
					}
				}
				else
				{
					if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::left)
						chunksInFront.push_back(chunk);
					
					if (curPlayer.camera.reverseDirection.y < 0)
					{
						if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::left + Vec3i::up)
							chunksInFront.push_back(chunk);
					}
					else
					{
						if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::left + Vec3i::down)
							chunksInFront.push_back(chunk);
					}
					
					if (curPlayer.camera.reverseDirection.z < 0)
					{
						if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::left + Vec3i::forward)
							chunksInFront.push_back(chunk);
					}
					else
					{
						if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::left + Vec3i::back)
							chunksInFront.push_back(chunk);
					}
				}
				
				if (curPlayer.camera.reverseDirection.y < 0)
				{
					if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::up)
						chunksInFront.push_back(chunk);
				}
				else
				{
					if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::down)
						chunksInFront.push_back(chunk);
				}
				
				if (curPlayer.camera.reverseDirection.z < 0)
				{
					if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::forward)
						chunksInFront.push_back(chunk);
					
					if (curPlayer.camera.reverseDirection.x < 0)
					{
						if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::forward + Vec3i::right)
							chunksInFront.push_back(chunk);
					}
					else
					{
						if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::forward + Vec3i::left)
							chunksInFront.push_back(chunk);
					}
					
					if (curPlayer.camera.reverseDirection.y < 0)
					{
						if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::forward + Vec3i::up)
							chunksInFront.push_back(chunk);
					
						if (curPlayer.camera.reverseDirection.x < 0)
						{
							if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::forward + Vec3i::up + Vec3i::right)
								chunksInFront.push_back(chunk);
						}
						else
						{
							if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::forward + Vec3i::up + Vec3i::left)
								chunksInFront.push_back(chunk);
						}
					}
					else
					{
						if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::forward + Vec3i::down)
							chunksInFront.push_back(chunk);
					
						if (curPlayer.camera.reverseDirection.x < 0)
						{
							if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::forward + Vec3i::down + Vec3i::right)
								chunksInFront.push_back(chunk);
						}
						else
						{
							if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::forward + Vec3i::down + Vec3i::left)
								chunksInFront.push_back(chunk);
						}
					}
				}
				else
				{
					if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::back)
						chunksInFront.push_back(chunk);
					
					if (curPlayer.camera.reverseDirection.x < 0)
					{
						if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::back + Vec3i::right)
							chunksInFront.push_back(chunk);
					}
					else
					{
						if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::back + Vec3i::left)
							chunksInFront.push_back(chunk);
					}
					
					if (curPlayer.camera.reverseDirection.y < 0)
					{
						if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::back + Vec3i::up)
							chunksInFront.push_back(chunk);
					
						if (curPlayer.camera.reverseDirection.x < 0)
						{
							if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::back + Vec3i::up + Vec3i::right)
								chunksInFront.push_back(chunk);
						}
						else
						{
							if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::back + Vec3i::up + Vec3i::left)
								chunksInFront.push_back(chunk);
						}
					}
					else
					{
						if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::back + Vec3i::down)
							chunksInFront.push_back(chunk);
					
						if (curPlayer.camera.reverseDirection.x < 0)
						{
							if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::back + Vec3i::down + Vec3i::right)
								chunksInFront.push_back(chunk);
						}
						else
						{
							if (chunkPos == chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk) + Vec3i::back + Vec3i::down + Vec3i::left)
								chunksInFront.push_back(chunk);
						}
					}
				}
			}

			//Sorts the chunks in front by proximity
			for (size_t i = 1; i < chunksInFront.size(); ++i)
			{
				const auto& chunkPos = Vec3f(chunkManager_.chunkPosManager.GetComponent(chunksInFront[i]) * kChunkSize) + Vec3f(kChunkSize / 2.0f);
				for (size_t j = 1; j < chunksInFront.size(); ++j)
				{
					if (chunksInFront[j] == chunksInFront[i]) continue;
					
					const auto& otherChunkPos = Vec3f(chunkManager_.chunkPosManager.GetComponent(chunksInFront[j]) * kChunkSize) + Vec3f(kChunkSize / 2.0f);
					if ((chunkPos - curPlayer.position).SquareMagnitude() < (otherChunkPos - curPlayer.position).SquareMagnitude())
						std::swap(chunksInFront[i], chunksInFront[j]);
				}
			}
			
			Ray rayOut;
			const Vec3f direction = curPlayer.camera.reverseDirection.Normalized() * -1;
			for (auto& chunkInFront : chunksInFront)
			{
				if (aabbManager_.RaycastBlockInChunk(rayOut, curPlayer.position, direction, chunkInFront))
				{
					if (rayOut.hitDist > curPlayer.maxReach) break;
					
					glLineWidth(3.0f);
					gizmosRenderer_.DrawCube(rayOut.hitAabb.CalculateCenter(), Vec3f(kCubeHalfSize + 0.01f) * 2, Color::black);

					if (Time::time > curPlayer.placeTimeStamp)
					{
						if (inputManager_.IsMouseButtonDown(sdl::MouseButtonCode::LEFT))
						{
							curPlayer.placeTimeStamp = Time::time + curPlayer.placeCoolDown;
							BreakBlock(chunkInFront, rayOut.hitId);
						}
						
						if (inputManager_.IsMouseButtonDown(sdl::MouseButtonCode::RIGHT))
						{
							curPlayer.placeTimeStamp = Time::time + curPlayer.placeCoolDown;
							PlaceBlock(chunkInFront, rayOut.hitId, blockManager_.GetRandomBlock());
						}
					}
					break;
				}
			}
		}
	}
	else
	{
		LogError("Current Player not set!");
	}
}

void PlayerController::FixedUpdate()
{
	const auto players = entityManager_.FilterEntities(static_cast<EntityMask>(ComponentType::PLAYER));
	for (auto& player : players)
	{
		Player curPlayer = playerManager_.GetComponent(player);
		curPlayer.camera.FixedUpdate();
	}
}

void PlayerController::OnEvent(const SDL_Event& event)
{
	const auto players = entityManager_.FilterEntities(static_cast<EntityMask>(ComponentType::PLAYER));
	for (auto& player : players)
	{
		Player curPlayer = playerManager_.GetComponent(player);
		curPlayer.camera.OnEvent(event);
	}
}

void PlayerController::Destroy()
{
}

void PlayerController::PlaceBlock(const Entity chunk, const BlockId blockId, const Block& block) const
{
	chunkManager_.chunkContentManager.SetBlock(chunk, blockId, block);
	chunkManager_.chunkRenderManager.SetChunkValues(chunk);
}

void PlayerController::BreakBlock(const Entity chunk, const BlockId blockId) const
{
	chunkManager_.chunkContentManager.RemoveBlock(chunk, blockId);
}

void PlayerController::SetCurrentPlayer(const Entity player)
{
	currentPlayer_ = player;
}

Entity PlayerController::GetCurrentPlayer() const
{
	return currentPlayer_;
}
}
