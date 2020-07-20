#include "minelib/player/player_controller.h"

#include "minelib/blocks/block.h"
#include "minelib/chunks/chunk.h"
#include "minelib/gizmos_renderer.h"
#include "minelib/aabb_manager.h"

namespace neko
{
PlayerController::PlayerController(MinecraftLikeEngine& engine)
	: engine_(engine),
	  entityManager_(engine.entityManager),
	  blockManager_(engine.blockManager),
	  uiManager_(engine.uiManager),
	  playerManager_(engine.componentsManagerSystem.playerManager),
	  chunkManager_(engine.componentsManagerSystem.chunkManager),
	  aabbManager_(engine.componentsManagerSystem.aabbManager),
	  inputManager_(sdl::InputLocator::get()),
	  gizmosRenderer_(GizmosLocator::get())
{
}

void PlayerController::Init()
{
	const auto& config = BasicEngine::GetInstance()->config;
	const Vec2f toolBarSize = Vec2f(ui_.hotBar.size) / Vec2f(config.windowSize);
	ui_.hotBar.position.y = toolBarSize.y / 2 - 1.0;
	
	const Vec2f tileSize = Vec2f(kTileSize) / Vec2f(config.windowSize);
	
	ui_.blockSelect.position.y = ui_.hotBar.position.y;
	ui_.blockSelect.position.x = ui_.hotBar.position.x + (ui_.selectIndex - 4) * tileSize.x;
	
	ui_.crossHair.texturePath = config.dataRootPath + "sprites/ui/crosshair.png";
	ui_.hotBar.texturePath = config.dataRootPath + "sprites/ui/toolbar.png";
	ui_.blockSelect.texturePath = config.dataRootPath + "sprites/ui/selection_sprite.png";
	uiManager_.AddUiElement(ui_.crossHair);
	uiManager_.AddUiElement(ui_.hotBar);
	for (int i = 0; i < ui_.hotBarPreviews.size(); ++i)
	{
		ui_.hotBarPreviews[i].position.x = ui_.hotBar.position.x + (i - 4) * tileSize.x;
		ui_.hotBarPreviews[i].position.y = ui_.hotBar.position.y;
		ui_.hotBarPreviews[i].size = kTileSize;

		if (ui_.hotBarBlocks[i] > 0)
			ui_.hotBarPreviews[i].textureId = blockManager_.GetBlock(ui_.hotBarBlocks[i]).previewTexture;
		else
			ui_.hotBarPreviews[i].texturePath = config.dataRootPath + "sprites/empty.png";

		uiManager_.AddUiElement(ui_.hotBarPreviews[i]);
	}
	uiManager_.AddUiElement(ui_.blockSelect);
}

void PlayerController::Update(const seconds dt)
{
	const auto players = entityManager_.FilterEntities(static_cast<EntityMask>(ComponentType::PLAYER));
	for (auto& player : players)
	{
		Player curPlayer = playerManager_.GetComponent(player);
		curPlayer.camera.Update(dt);
		curPlayer.position = curPlayer.camera.position;
		
		const Vec3i currentChunkPos = Vec3i(
			std::floor(curPlayer.position.x / kChunkSize),
			std::floor(curPlayer.position.y / kChunkSize),
			std::floor(curPlayer.position.z / kChunkSize)
		);
		if (curPlayer.currentChunk == INVALID_ENTITY || 
			currentChunkPos != chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk))
		{
			const auto chunks = chunkManager_.chunkStatusManager.GetAccessibleChunks();
			for (auto& chunk : chunks)
			{
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
			const auto chunks = chunkManager_.chunkStatusManager.GetAccessibleChunks();
			for (auto& chunk : chunks)
			{
				auto& chunkPos = chunkManager_.chunkPosManager.GetComponent(chunk);
				auto& curChunkPos = chunkManager_.chunkPosManager.GetComponent(curPlayer.currentChunk);
				if (curPlayer.camera.reverseDirection.x < 0)
				{
					if (chunkPos == curChunkPos + Vec3i::right)
						chunksInFront.push_back(chunk);
					
					if (curPlayer.camera.reverseDirection.y < 0)
					{
						if (chunkPos == curChunkPos + Vec3i::right + Vec3i::up)
							chunksInFront.push_back(chunk);
					}
					else
					{
						if (chunkPos == curChunkPos + Vec3i::right + Vec3i::down)
							chunksInFront.push_back(chunk);
					}
					
					if (curPlayer.camera.reverseDirection.z < 0)
					{
						if (chunkPos == curChunkPos + Vec3i::right + Vec3i::forward)
							chunksInFront.push_back(chunk);
					}
					else
					{
						if (chunkPos == curChunkPos + Vec3i::right + Vec3i::back)
							chunksInFront.push_back(chunk);
					}
				}
				else
				{
					if (chunkPos == curChunkPos + Vec3i::left)
						chunksInFront.push_back(chunk);
					
					if (curPlayer.camera.reverseDirection.y < 0)
					{
						if (chunkPos == curChunkPos + Vec3i::left + Vec3i::up)
							chunksInFront.push_back(chunk);
					}
					else
					{
						if (chunkPos == curChunkPos + Vec3i::left + Vec3i::down)
							chunksInFront.push_back(chunk);
					}
					
					if (curPlayer.camera.reverseDirection.z < 0)
					{
						if (chunkPos == curChunkPos + Vec3i::left + Vec3i::forward)
							chunksInFront.push_back(chunk);
					}
					else
					{
						if (chunkPos == curChunkPos + Vec3i::left + Vec3i::back)
							chunksInFront.push_back(chunk);
					}
				}
				
				if (curPlayer.camera.reverseDirection.y < 0)
				{
					if (chunkPos == curChunkPos + Vec3i::up)
						chunksInFront.push_back(chunk);
				}
				else
				{
					if (chunkPos == curChunkPos + Vec3i::down)
						chunksInFront.push_back(chunk);
				}
				
				if (curPlayer.camera.reverseDirection.z < 0)
				{
					if (chunkPos == curChunkPos + Vec3i::forward)
						chunksInFront.push_back(chunk);
					
					if (curPlayer.camera.reverseDirection.x < 0)
					{
						if (chunkPos == curChunkPos + Vec3i::forward + Vec3i::right)
							chunksInFront.push_back(chunk);
					}
					else
					{
						if (chunkPos == curChunkPos + Vec3i::forward + Vec3i::left)
							chunksInFront.push_back(chunk);
					}
					
					if (curPlayer.camera.reverseDirection.y < 0)
					{
						if (chunkPos == curChunkPos + Vec3i::forward + Vec3i::up)
							chunksInFront.push_back(chunk);
					
						if (curPlayer.camera.reverseDirection.x < 0)
						{
							if (chunkPos == curChunkPos + Vec3i::forward + Vec3i::up + Vec3i::right)
								chunksInFront.push_back(chunk);
						}
						else
						{
							if (chunkPos == curChunkPos + Vec3i::forward + Vec3i::up + Vec3i::left)
								chunksInFront.push_back(chunk);
						}
					}
					else
					{
						if (chunkPos == curChunkPos + Vec3i::forward + Vec3i::down)
							chunksInFront.push_back(chunk);
					
						if (curPlayer.camera.reverseDirection.x < 0)
						{
							if (chunkPos == curChunkPos + Vec3i::forward + Vec3i::down + Vec3i::right)
								chunksInFront.push_back(chunk);
						}
						else
						{
							if (chunkPos == curChunkPos + Vec3i::forward + Vec3i::down + Vec3i::left)
								chunksInFront.push_back(chunk);
						}
					}
				}
				else
				{
					if (chunkPos == curChunkPos + Vec3i::back)
						chunksInFront.push_back(chunk);
					
					if (curPlayer.camera.reverseDirection.x < 0)
					{
						if (chunkPos == curChunkPos + Vec3i::back + Vec3i::right)
							chunksInFront.push_back(chunk);
					}
					else
					{
						if (chunkPos == curChunkPos + Vec3i::back + Vec3i::left)
							chunksInFront.push_back(chunk);
					}
					
					if (curPlayer.camera.reverseDirection.y < 0)
					{
						if (chunkPos == curChunkPos + Vec3i::back + Vec3i::up)
							chunksInFront.push_back(chunk);
					
						if (curPlayer.camera.reverseDirection.x < 0)
						{
							if (chunkPos == curChunkPos + Vec3i::back + Vec3i::up + Vec3i::right)
								chunksInFront.push_back(chunk);
						}
						else
						{
							if (chunkPos == curChunkPos + Vec3i::back + Vec3i::up + Vec3i::left)
								chunksInFront.push_back(chunk);
						}
					}
					else
					{
						if (chunkPos == curChunkPos + Vec3i::back + Vec3i::down)
							chunksInFront.push_back(chunk);
					
						if (curPlayer.camera.reverseDirection.x < 0)
						{
							if (chunkPos == curChunkPos + Vec3i::back + Vec3i::down + Vec3i::right)
								chunksInFront.push_back(chunk);
						}
						else
						{
							if (chunkPos == curChunkPos + Vec3i::back + Vec3i::down + Vec3i::left)
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
					
					gizmosRenderer_.DrawCube(rayOut.hitAabb.CalculateCenter(), Vec3f(kCubeHalfSize + 0.01f) * 2, Color::black, 3.0f);

					if (Time::time > curPlayer.placeTimeStamp)
					{
						if (inputManager_.IsMouseButtonHeld(sdl::MouseButtonCode::LEFT))
						{
							curPlayer.placeTimeStamp = Time::time + curPlayer.placeCoolDown;
							BreakBlock(chunkInFront, rayOut.hitId);
						}
						
						if (inputManager_.IsMouseButtonHeld(sdl::MouseButtonCode::RIGHT))
						{
							if (ui_.hotBarBlocks[ui_.selectIndex] <= 0) break;
							
							curPlayer.placeTimeStamp = Time::time + curPlayer.placeCoolDown;
							
							const Vec3f chunkPos = Vec3f(chunkManager_.chunkPosManager.GetComponent(chunkInFront));
							const Vec3f hitPos = Vec3f(BlockIdToPos(rayOut.hitId)) + chunkPos * kChunkSize;
							
							const Vec3f toPoint = curPlayer.camera.reverseDirection * -1 * rayOut.hitDist;
							const Vec3f cubePoint = curPlayer.camera.position + toPoint;
							const Vec3f fromCenter = cubePoint - hitPos;
							
							Vec3i offset;
							if (Abs(fromCenter.x) > Abs(fromCenter.y) && 
								Abs(fromCenter.x) > Abs(fromCenter.z)) 
								offset.x = -Sign(fromCenter.x);
							else if (Abs(fromCenter.y) > Abs(fromCenter.x) && 
								Abs(fromCenter.y) > Abs(fromCenter.z)) 
								offset.y = -Sign(fromCenter.y);
							else if (Abs(fromCenter.z) > Abs(fromCenter.y) && 
								Abs(fromCenter.z) > Abs(fromCenter.x)) 
								offset.z = -Sign(fromCenter.z);
							
							const Block& block = blockManager_.GetBlock(ui_.hotBarBlocks[ui_.selectIndex]);
							PlaceBlock(chunkInFront, rayOut.hitId - PosToBlockId(offset), block);
						}
						
						if (inputManager_.IsMouseButtonHeld(sdl::MouseButtonCode::MIDDLE))
						{
							const ChunkContent& chunkBlock = chunkManager_.chunkContentManager.GetBlock(chunkInFront, rayOut.hitId);
							const Block& block = blockManager_.GetBlockByTexHash(chunkBlock.texId);
							ui_.hotBarBlocks[ui_.selectIndex] = block.id;
							ui_.hotBarPreviews[ui_.selectIndex].textureId = block.previewTexture;
							ui_.hotBarPreviews[ui_.selectIndex].flags |= UiElement::DIRTY;
						}
					}
					break;
				}
			}
			
			playerManager_.SetComponent(currentPlayer_, curPlayer);
		}
	}
	else
	{
		LogError("Current Player not set!");
	}

	UpdateUi();
}

void PlayerController::UpdateUi()
{
	const auto scrollAmount = inputManager_.GetMouseScroll();
	if (scrollAmount.y != 0)
	{
		ui_.selectIndex += -Sign(scrollAmount.y);
		if (ui_.selectIndex > kHotBarSize - 1) ui_.selectIndex = 0;
		if (ui_.selectIndex < 0) ui_.selectIndex = kHotBarSize - 1;
		
		const auto& config = BasicEngine::GetInstance()->config;
		const Vec2f tileSize = Vec2f(kTileSize) / Vec2f(config.windowSize);
		ui_.blockSelect.position.x = ui_.hotBar.position.x + (ui_.selectIndex - 4) * tileSize.x;
		ui_.blockSelect.position.y = ui_.hotBar.position.y;
		ui_.blockSelect.flags |= UiElement::DIRTY;
	}
}

void PlayerController::FixedUpdate()
{
	const auto players = entityManager_.FilterEntities(static_cast<EntityMask>(ComponentType::PLAYER));
	for (auto& player : players)
	{
		Player curPlayer = playerManager_.GetComponent(player);
		curPlayer.camera.FixedUpdate();
		curPlayer.position = curPlayer.camera.position;
		playerManager_.SetComponent(player, curPlayer);
	}
}

void PlayerController::OnEvent(const SDL_Event& event)
{
	const auto players = entityManager_.FilterEntities(static_cast<EntityMask>(ComponentType::PLAYER));
	for (auto& player : players)
	{
		Player curPlayer = playerManager_.GetComponent(player);
		curPlayer.camera.OnEvent(event);
		curPlayer.position = curPlayer.camera.position;
		playerManager_.SetComponent(player, curPlayer);
	}
	
	if(event.window.event == SDL_WINDOWEVENT_RESIZED)
	{
		const Vec2f newWindowSize = Vec2f(event.window.data1, event.window.data2);
		{
			const Vec2f normalSpaceSize = Vec2f(ui_.hotBar.size) / newWindowSize;
			ui_.hotBar.position.y = normalSpaceSize.y / 2 - 1.0;
		}
		{
			const Vec2f tileSize = Vec2f(kTileSize) / newWindowSize;
			for (auto i = 0; i < kHotBarSize; ++i)
			{
				if (ui_.hotBarBlocks[i] <= 0) continue;
				ui_.hotBarPreviews[i].position.x = ui_.hotBar.position.x + (i - 4) * tileSize.x;
				ui_.hotBarPreviews[i].position.y = ui_.hotBar.position.y;
			}
			
			ui_.blockSelect.position.y = ui_.hotBar.position.y;
			ui_.blockSelect.position.x = ui_.hotBar.position.x + (ui_.selectIndex - 4) * tileSize.x;
		}
	}
}

void PlayerController::Destroy()
{
}

void PlayerController::PlaceBlock(const Entity chunk, const BlockId blockId, const Block& block) const
{
	chunkManager_.chunkContentManager.SetBlock(chunk, block, blockId);
	chunkManager_.chunkStatusManager.AddStatus(chunk, ChunkFlag::DIRTY);
}

void PlayerController::BreakBlock(const Entity chunk, const BlockId blockId) const
{
	chunkManager_.chunkContentManager.RemoveBlock(chunk, blockId);
	chunkManager_.chunkStatusManager.AddStatus(chunk, ChunkFlag::DIRTY);
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
