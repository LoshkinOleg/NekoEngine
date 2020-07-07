#include "minelib/player/player_controller.h"

namespace neko
{
PlayerController::PlayerController(MinecraftLikeEngine& engine)
	: inputManager_(sdl::InputLocator::get()),
	  gizmosRenderer_(GizmosLocator::get()),
	  chunksManager_(engine.componentsManagerSystem_.chunkManager_)
{
}

void PlayerController::Init()
{
	player_.aabb.SetFromCenter(player_.pos + Vec3f::up, player_.aabbSize);
}

void PlayerController::Update(seconds dt)
{
	camera_.Update(dt);

	//TODO implement hotbar
	/*const auto scrollAmount = inputManager_.GetMouseScroll();
	if (scrollAmount.y != 0)
	{
		selectIndex_ += -Sign(scrollAmount.y);
		if (selectIndex_ > toolbarSize - 1) selectIndex_ = 0;
		if (selectIndex_ < 0) selectIndex_ = toolbarSize - 1;
		
		const auto& config = BasicEngine::GetInstance()->config;
		const Vec2f tileSize = Vec2f(tileSize_) / Vec2f(config.windowSize);
		blockSelect_.position.x = toolBar_.position.x + (selectIndex_ - 4) * tileSize.x;
		blockSelect_.position.y = toolBar_.position.y;
	}*/
	
	if (inputManager_.IsActionDown(sdl::InputAction::SPRINT) && inputManager_.IsActionHeld(sdl::InputAction::UP))
	{
		player_.speedMultiplier = player_.sprintMultiplier;
		player_.state &= Player::SPRINTING;
	}

	if (inputManager_.IsActionUp(sdl::InputAction::UP))
	{
		player_.state |= ~Player::SPRINTING;
		player_.speedMultiplier = 1.0f;
	}
	
	if (player_.state & Player::CAN_JUMP && inputManager_.IsActionDown(sdl::InputAction::JUMP))
	{
		player_.velocity.y = player_.jumpPower;
		player_.state |= ~Player::CAN_JUMP;
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

void PlayerController::PlaceCube(size_t chunkId, size_t blockId)
{
	//TODO 
}

void PlayerController::DeleteCube(size_t chunkId, size_t blockId)
{
	//TODO 
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

	player_.pos += camera_.GetRight() * player_.velocity.x +
		Vec3f::up * player_.velocity.y * Time::fixedDeltaTime - 
		Vec3f::Cross(camera_.GetRight(), Vec3f::up) * player_.velocity.z;
	player_.aabb.SetFromCenter(player_.pos + Vec3f::up, player_.aabbSize);
	
	camera_.position = player_.pos + player_.cameraOffset;
}

void PlayerController::ResolvePhysics()
{
	/*Ray rayOut;
	if (RayCast(rayOut, player_.pos + Vec3f::up * 0.01f, Vec3f::down, player_.maxReach))
	{
		if (Equal(rayOut.hitDist, 0.01f, 0.001f)) 
			player_.canJump = true;
		else
			player_.canJump = false;
	}
	
	for (size_t i = 0; i < cubeAabbs_.size(); ++i)
	{
		const Vec3f fromCenter = cubePositions_[i] - Vec3f(player_.pos.x, cubePositions_[i].y - cubeHalfSize, player_.pos.z);
		if (cubeAabbs_[i].IntersectAabb(Aabb3d(player_.pos + Vec3f(player_.velocity.x, cubeHalfSize * 2, player_.velocity.z), player_.aabbSize)) &&
			Abs(fromCenter.x) > Abs(fromCenter.y) && 
			Abs(fromCenter.x) > Abs(fromCenter.z))
		{
			const float sideSign = Sign(cubePositions_[i].x - player_.pos.x);
			player_.pos.x = cubePositions_[i].x - (cubeHalfSize + player_.aabbSize.x + 0.01f) * sideSign;
		}
		else if (cubeAabbs_[i].ContainsPoint(player_.pos + Vec3f(0, player_.velocity.y * Time::fixedDeltaTime, 0)) &&
			Abs(fromCenter.y) > Abs(fromCenter.x) && 
			Abs(fromCenter.y) > Abs(fromCenter.z))
		{
			const float sideSign = Sign(cubePositions_[i].y - player_.pos.y);
			player_.pos.y = cubePositions_[i].y - cubeHalfSize * sideSign;

			if (player_.velocity.y <= 0.0f) player_.velocity.y = 0.0f;
		}
		else if (cubeAabbs_[i].IntersectAabb(Aabb3d(player_.pos + Vec3f(player_.velocity.x, cubeHalfSize * 2, player_.velocity.z), player_.aabbSize)) &&
			Abs(fromCenter.z) > Abs(fromCenter.y) && 
			Abs(fromCenter.z) > Abs(fromCenter.x))
		{
			const float sideSign = Sign(cubePositions_[i].z - player_.pos.z);
			player_.pos.z = cubePositions_[i].z - (cubeHalfSize + player_.aabbSize.z + 0.01f) * sideSign;
		}
	}*/
}

void PlayerController::HeadBobbing()
{
	if (IsMoving())
    {
        player_.cameraOffset = camera_.GetRight() * std::sin(player_.bobbingTimer / 2) * player_.bobbingAmount;
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

void PlayerController::CheckBlock()
{
	/*Ray rayOut;
	const Vec3f direction = camera_.reverseDirection.Normalized() * -1;
	if (RayCast(rayOut, camera_.position, direction, player_.maxReach))
	{
		glLineWidth(3.0f);
		gizmosRenderer_.DrawCube(cubePositions_[rayOut.hitIndex], Vec3f::one, Color::black);
		
		if (Time::time < player_.placeTimeStamp) return;
		if (inputManager_.IsMouseButtonHeld(sdl::MouseButtonCode::LEFT))
		{
			DeleteCube(rayOut.hitIndex);
		}
		if (inputManager_.IsMouseButtonHeld(sdl::MouseButtonCode::RIGHT) && toolBarBlockIds_[selectIndex_] != -1)
		{
			const Vec3f toPoint = camera_.reverseDirection * -1 * rayOut.hitDist;
			const Vec3f cubePoint = camera_.position + toPoint;
			const Vec3f fromCenter = cubePoint - cubePositions_[rayOut.hitIndex];
			
			Vec3f offset;
			if (Abs(fromCenter.x) > Abs(fromCenter.y) && 
				Abs(fromCenter.x) > Abs(fromCenter.z)) 
				offset.x = -Sign(fromCenter.x);
			else if (Abs(fromCenter.y) > Abs(fromCenter.x) && 
				Abs(fromCenter.y) > Abs(fromCenter.z)) 
				offset.y = -Sign(fromCenter.y);
			else if (Abs(fromCenter.z) > Abs(fromCenter.y) && 
				Abs(fromCenter.z) > Abs(fromCenter.x)) 
				offset.z = -Sign(fromCenter.z);

			PlaceCube(rayOut.hitPos - offset);
		}
	}*/
}

bool PlayerController::IsMoving() const
{
	return inputManager_.IsActionHeld(sdl::InputAction::RIGHT) ||
		inputManager_.IsActionHeld(sdl::InputAction::LEFT) ||
		inputManager_.IsActionHeld(sdl::InputAction::UP) ||
		inputManager_.IsActionHeld(sdl::InputAction::DOWN);
}
}
