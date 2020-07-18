#include "minelib/player/player_controller.h"

#include "minelib/blocks/block.h"
#include "minelib/chunks/chunk.h"

namespace neko
{
PlayerController::PlayerController(MinecraftLikeEngine& engine)
	: engine_(engine),
	  blockManager_(engine.blockManager),
	  chunkManager_(engine.componentsManagerSystem.chunkManager),
	  inputManager_(sdl::InputLocator::get()),
	  gizmosRenderer_(GizmosLocator::get()),
	  aabbManager_(AabbLocator::get()),
	  uiManager_(UiManagerLocator::get())
{
}

void PlayerController::Init()
{
	player_.aabb.SetFromCenter(player_.pos + Vec3f::up, player_.aabbSize);

	//Init Ui
	{
		const Vec2f toolBarSize = Vec2f(toolBar_.size) / Vec2f(engine_.config.windowSize);
		toolBar_.position.y = toolBarSize.y / 2 - 1.0;
	
		const Vec2f tileSize = Vec2f(tileSize_) / Vec2f(engine_.config.windowSize);
	
		blockSelect_.position.y = toolBar_.position.y;
		blockSelect_.position.x = toolBar_.position.x + (selectIndex_ - 4) * tileSize.x;

		crossHair_.texturePath = engine_.config.dataRootPath + "sprites/ui/crosshair.png";
		toolBar_.texturePath = engine_.config.dataRootPath + "sprites/ui/toolbar.png";
		blockSelect_.texturePath = engine_.config.dataRootPath + "sprites/ui/selection_sprite.png";
		uiManager_.AddUiElement(&crossHair_);
		uiManager_.AddUiElement(&toolBar_);
		uiManager_.AddUiElement(&blockSelect_);

		for (int i = 0; i < toolbarSize; ++i)
		{
			blockPreviews_[i].position.x = toolBar_.position.x + (i - 4) * tileSize.x;
			blockPreviews_[i].position.y = toolBar_.position.y;
			blockPreviews_[i].textureId = toolBarBlocks_[i]->previewTexture;
			uiManager_.AddUiElement(&blockPreviews_[i]);
		}
	}
}

void PlayerController::Update(seconds dt)
{
	camera_.Update(dt);

	const auto scrollAmount = inputManager_.GetMouseScroll();
	if (scrollAmount.y != 0)
	{
		selectIndex_ += -Sign(scrollAmount.y);
		if (selectIndex_ > toolbarSize - 1) selectIndex_ = 0;
		if (selectIndex_ < 0) selectIndex_ = toolbarSize - 1;
		
		const auto& config = BasicEngine::GetInstance()->config;
		const Vec2f tileSize = Vec2f(tileSize_) / Vec2f(config.windowSize);
		blockSelect_.position.x = toolBar_.position.x + (selectIndex_ - 4) * tileSize.x;
		blockSelect_.position.y = toolBar_.position.y;
		blockSelect_.Update(config.windowSize);
	}
	
	if (inputManager_.IsActionDown(sdl::InputAction::SPRINT) && inputManager_.IsActionHeld(sdl::InputAction::UP))
	{
		player_.speedMultiplier = player_.sprintMultiplier;
		player_.state |= Player::SPRINTING;
	}

	if (inputManager_.IsActionUp(sdl::InputAction::UP))
	{
		player_.state &= ~Player::SPRINTING;
		player_.speedMultiplier = 1.0f;
	}
	
	if (player_.state & Player::CAN_JUMP && inputManager_.IsActionDown(sdl::InputAction::JUMP))
	{
		player_.velocity.y = player_.jumpPower;
		player_.state &= ~Player::CAN_JUMP;
	}

	if (Equal(Abs(player_.velocity.y), 0.0f)) HeadBobbing();
}

void PlayerController::FixedUpdate()
{
	camera_.FixedUpdate();
	MovePlayer();
}

void PlayerController::Destroy()
{
}

void PlayerController::PlaceCube(const size_t chunkId, const size_t blockId) const
{
	chunkManager_.chunkContentManager.SetBlock(chunkId, blockManager_.GetBlock(0), blockId);
}

void PlayerController::DeleteCube(const size_t chunkId, const size_t blockId) const
{
	chunkManager_.chunkContentManager.RemoveBlock(chunkId, blockId);
}

void PlayerController::MovePlayer()
{
	if (inputManager_.IsActionHeld(sdl::InputAction::RIGHT))
		player_.velocity.x += Time::fixedDeltaTime * player_.speedMultiplier;
	if (inputManager_.IsActionHeld(sdl::InputAction::LEFT))
		player_.velocity.x -= Time::fixedDeltaTime * player_.speedMultiplier;

	if (inputManager_.IsActionHeld(sdl::InputAction::UP))
		player_.velocity.z += Time::fixedDeltaTime * player_.speedMultiplier;
	if (inputManager_.IsActionHeld(sdl::InputAction::DOWN))
		player_.velocity.z -= Time::fixedDeltaTime * player_.speedMultiplier;
	
	player_.velocity.x = Lerp(player_.velocity.x, 0.0f, player_.decelerationSpeed * Time::fixedDeltaTime);
	player_.velocity.z = Lerp(player_.velocity.z, 0.0f, player_.decelerationSpeed * Time::fixedDeltaTime);
	
	player_.velocity.y -= gravity_ * Time::fixedDeltaTime;
	
	ResolvePhysics();

	player_.pos += camera_.rightDirection * player_.velocity.x +
		Vec3f::up * player_.velocity.y * Time::fixedDeltaTime - 
		Vec3f::Cross(camera_.rightDirection, Vec3f::up) * player_.velocity.z;
	player_.aabb.SetFromCenter(player_.pos + Vec3f::up, player_.aabbSize);
	
	camera_.position = player_.pos + player_.cameraOffset;
}

void PlayerController::ResolvePhysics()
{
	Ray rayOut;
	if (aabbManager_.RaycastBlockInChunk(rayOut, player_.pos + Vec3f::up * 0.01f, Vec3f::down, 0))
	{
		if (Equal(rayOut.hitDist, 0.01f, 0.001f)) 
			player_.state |= Player::CAN_JUMP;
		else
			player_.state &= ~Player::CAN_JUMP;
	}
	
	for (uint16_t i = 0; i < uint16_t(kChunkSize * kChunkSize * kChunkSize); i++)
	{
		const uint16_t z = std::floor(i / (kChunkSize * kChunkSize));
		const uint16_t y = std::floor((i - z * kChunkSize * kChunkSize) / kChunkSize);
		const uint16_t x = i % kChunkSize;
		const Vec3f cubePos = Vec3f(x, y, z);
		const Aabb3d aabb = Aabb3d(cubePos, Vec3f(kCubeHalfSize));
		
		const Vec3f fromCenter = cubePos - Vec3f(player_.pos.x, cubePos.y - kCubeHalfSize, player_.pos.z);
		if (aabb.IntersectAabb(Aabb3d(player_.pos + Vec3f(player_.velocity.x, kCubeHalfSize * 2, player_.velocity.z), player_.aabbSize)) &&
			Abs(fromCenter.x) > Abs(fromCenter.y) && 
			Abs(fromCenter.x) > Abs(fromCenter.z))
		{
			const float sideSign = Sign(cubePos.x - player_.pos.x);
			player_.pos.x = cubePos.x - (kCubeHalfSize + player_.aabbSize.x + 0.01f) * sideSign;
		}
		else if (aabb.ContainsPoint(player_.pos + Vec3f(0, player_.velocity.y * Time::fixedDeltaTime, 0)) &&
			Abs(fromCenter.y) > Abs(fromCenter.x) && 
			Abs(fromCenter.y) > Abs(fromCenter.z))
		{
			const float sideSign = Sign(cubePos.y - player_.pos.y);
			player_.pos.y = cubePos.y - kCubeHalfSize * sideSign;

			if (player_.velocity.y <= 0.0f) player_.velocity.y = 0.0f;
		}
		else if (aabb.IntersectAabb(Aabb3d(player_.pos + Vec3f(player_.velocity.x, kCubeHalfSize * 2, player_.velocity.z), player_.aabbSize)) &&
			Abs(fromCenter.z) > Abs(fromCenter.y) && 
			Abs(fromCenter.z) > Abs(fromCenter.x))
		{
			const float sideSign = Sign(cubePos.z - player_.pos.z);
			player_.pos.z = cubePos.z - (kCubeHalfSize + player_.aabbSize.z + 0.01f) * sideSign;
		}
	}
}

void PlayerController::HeadBobbing()
{
	if (IsMoving())
    {
        player_.cameraOffset = camera_.rightDirection * std::sin(player_.bobbingTimer / 2) * player_.bobbingAmount;
        player_.cameraOffset.y = player_.defaultPosY + std::sin(player_.bobbingTimer) * player_.bobbingAmount;
        player_.bobbingTimer += player_.bobbingSpeed * player_.speedMultiplier * Time::deltaTime;
    }
    else
    {
        player_.bobbingTimer = 0;
        player_.cameraOffset.x = Lerp(player_.cameraOffset.x, 0.0f, player_.bobbingSpeed * player_.speedMultiplier * Time::deltaTime);
        player_.cameraOffset.y = Lerp(player_.cameraOffset.y, player_.defaultPosY, player_.bobbingSpeed * player_.speedMultiplier * Time::deltaTime);
        player_.cameraOffset.z = Lerp(player_.cameraOffset.z, 0.0f, player_.bobbingSpeed * player_.speedMultiplier * Time::deltaTime);

    	if (Equal(player_.cameraOffset.x, 0.0f, 0.0001f)) player_.cameraOffset.x = 0.0f;
    	if (Equal(player_.cameraOffset.z, 0.0f, 0.0001f)) player_.cameraOffset.z = 0.0f;
    }
}

void PlayerController::CheckBlock(const uint16_t blockId, const Vec3f& blockPos) const
{
	glLineWidth(3.0f);
	gizmosRenderer_.DrawCube(blockPos, Vec3f::one, Color::black);
	
	if (Time::time < player_.placeTimeStamp) return;
	if (inputManager_.IsMouseButtonHeld(sdl::MouseButtonCode::LEFT))
	{
		DeleteCube(0, blockId);
	}
	if (inputManager_.IsMouseButtonHeld(sdl::MouseButtonCode::RIGHT) && toolBarBlocks_[selectIndex_] != nullptr)
	{
		const Vec3f toPoint = camera_.reverseDirection * -1 * blockId;
		const Vec3f cubePoint = camera_.position + toPoint;
		const Vec3f fromCenter = cubePoint - blockPos;
		
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

		const uint16_t offSetId = blockId - PosToBlockId(offset);
		PlaceCube(0, offSetId);
	}
}

bool PlayerController::IsMoving() const
{
	return inputManager_.IsActionHeld(sdl::InputAction::RIGHT) ||
		inputManager_.IsActionHeld(sdl::InputAction::LEFT) ||
		inputManager_.IsActionHeld(sdl::InputAction::UP) ||
		inputManager_.IsActionHeld(sdl::InputAction::DOWN);
}
}
