#include <xx_player_controller/player_program.h>

#include <sdl_engine/sdl_input.h>
#include <stb_image.h>

#include <engine/engine.h>
#include <gl/texture.h>

namespace neko
{
void PlayerProgram::Init()
{
	stbi_set_flip_vertically_on_load(true);
	
	const auto& config = BasicEngine::GetInstance()->config;
	shader_.LoadFromFile(
		config.dataRootPath + "shaders/base_instance.vert",
		config.dataRootPath + "shaders/base.frag");
	texture_ = gl::stbCreateTexture(config.dataRootPath + "sprites/wall.jpg");
	
	shaderLine_.LoadFromFile(
		config.dataRootPath + "shaders/xx_player_controller/gizmoLine.vert",
		config.dataRootPath + "shaders/xx_player_controller/gizmoLine.frag");
	selectCube_.Init();
	
	uiShader_.LoadFromFile(
		config.dataRootPath + "shaders/base_ui.vert",
		config.dataRootPath + "shaders/base.frag");
	
	std::array<TextureId, 3> cubeTex{};
	cubeTex[0] = stbCreateTexture(config.dataRootPath + "sprites/blocks/grass_side.png", gl::Texture::CLAMP_WRAP);
	cubeTex[1] = stbCreateTexture(config.dataRootPath + "sprites/blocks/grass_top.png", gl::Texture::CLAMP_WRAP);
	cubeTex[2] = stbCreateTexture(config.dataRootPath + "sprites/blocks/dirt.png", gl::Texture::CLAMP_WRAP);
	uniqueCube_.SetTextures(cubeTex);

	blockManager_.Init();
	
	std::fill(toolBarBlockIds_.begin(), toolBarBlockIds_.end(), -1);
	toolBarBlockIds_[0] = 0;
	toolBarBlockIds_[1] = 1;
	toolBarBlockIds_[2] = 2;
	toolBarBlockIds_[3] = 3;
	std::fill(blockPreviews_.begin(), blockPreviews_.end(), UiElement(Vec3f::zero, Vec2u(80, 80)));
	
	uiManager_.Init();
	{
		const Vec2f toolBarSize = Vec2f(toolBar_.size) / Vec2f(config.windowSize);
		toolBar_.position.y = toolBarSize.y / 2 - 1.0;
	
		const Vec2f tileSize = Vec2f(tileSize_) / Vec2f(config.windowSize);
	
		blockSelect_.position.y = toolBar_.position.y;
		blockSelect_.position.x = toolBar_.position.x + (selectIndex_ - 4) * tileSize.x;

		crossHair_.texturePath = config.dataRootPath + "sprites/ui/crosshair.png";
		toolBar_.texturePath = config.dataRootPath + "sprites/ui/toolbar.png";
		blockSelect_.texturePath = config.dataRootPath + "sprites/ui/selection_sprite.png";
		uiManager_.AddUiElement(&crossHair_);
		uiManager_.AddUiElement(&toolBar_);
		uiManager_.AddUiElement(&blockSelect_);

		for (int i = 0; i < toolbarSize; ++i)
		{
			if (toolBarBlockIds_[i] == -1) continue;
			
			blockPreviews_[i].position.x = toolBar_.position.x + (i - 4) * tileSize.x;
			blockPreviews_[i].position.y = toolBar_.position.y;
			blockPreviews_[i].textureId = blockManager_.GetBlock(toolBarBlockIds_[i])->previewTexture;
			uiManager_.AddUiElement(&blockPreviews_[i]);
		}
	}

	const auto start = std::chrono::high_resolution_clock::now();
	for (auto& chunk : chunks_)
	{
		for (uint16_t x = 0; x < kChunkSize; ++x)
		{
			for (uint16_t y = 0; y < kChunkSize; ++y)
			{
				for (uint16_t z = 0; z < kChunkSize; ++z)
				{
					chunk.SetBlock(1, Vec3i(x, y, z));
				}
			}
		}
	}
	const auto stop = std::chrono::high_resolution_clock::now();
	std::cout << (stop - start).count() << '\n';
	
	camera_.Init();
	camera_.position = playerPos_ + Vec3f::up;
	camera_.farPlane = 10'000.0f;

	playerAabb_.SetFromCenter(playerPos_ + Vec3f::up, playerAabbSize_);

	for (int x = -32; x < 32; ++x)
	{
		for (int z = -32; z < 32; ++z)
		{
			CreateCube(Vec3f(x, 0, z));
		}
	}
	uniqueCube_.InitInstanced(cubePositions_[0], cubePositions_.size());

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void PlayerProgram::Update(const seconds dt)
{
	std::lock_guard<std::mutex> lock(updateMutex_);

	camera_.Update(dt);
	
	const auto& inputManager = static_cast<sdl::InputManager&>(sdl::InputLocator::get());
	const auto scrollAmount = inputManager.GetMouseScroll();
	if (scrollAmount.y != 0)
	{
		selectIndex_ += -Sign(scrollAmount.y);
		if (selectIndex_ > toolbarSize - 1) selectIndex_ = 0;
		if (selectIndex_ < 0) selectIndex_ = toolbarSize - 1;
		
		const auto& config = BasicEngine::GetInstance()->config;
		const Vec2f tileSize = Vec2f(tileSize_) / Vec2f(config.windowSize);
		blockSelect_.position.x = toolBar_.position.x + (selectIndex_ - 4) * tileSize.x;
		blockSelect_.position.y = toolBar_.position.y;
	}
	
	if (inputManager.IsActionDown(sdl::InputAction::SPRINT) && inputManager.IsActionHeld(sdl::InputAction::UP))
	{
		speedMultiplier_ = sprintMultiplier_;
		playerState_ &= SPRINTING;
	}

	if (inputManager.IsActionUp(sdl::InputAction::UP))
	{
		playerState_ |= ~SPRINTING;
		speedMultiplier_ = 1.0f;
	}
	
	if (canJump_ && inputManager.IsActionDown(sdl::InputAction::JUMP))
	{
		playerVelocity_.y = playerJumpPower_;
		canJump_ = false;
		timer_ = 0.0f;
	}

	if (Equal(Abs(playerVelocity_.y), 0.0f)) HeadBobbing();
	
	if (playerVelocity_.y < 0.0f) timer_ += dt.count();
	if (playerPos_.y < -50.0f)
	{
		playerPos_ = Vec3f(0.0f, 1.5f, 0.0f);
		playerVelocity_ = Vec3f::zero;
		camera_.reverseDirection = Vec3f::back;
	}
}

void PlayerProgram::FixedUpdate()
{
	std::lock_guard<std::mutex> lock(updateMutex_);
	test_ = Time::time - testStamp_;
	testStamp_ = Time::time;

	camera_.FixedUpdate();
	MovePlayer();
}

void PlayerProgram::Render()
{
	if (shader_.GetProgram() == 0) return;
	if (texture_ == INVALID_TEXTURE_ID) return;

	std::lock_guard<std::mutex> lock(updateMutex_);

	shader_.Bind();
	glBindTexture(GL_TEXTURE_2D, texture_); //bind texture id to texture slot
	shader_.SetMat4("view", camera_.GenerateViewMatrix());
	shader_.SetMat4("projection", camera_.GenerateProjectionMatrix());
	
	CheckBlock();

	shader_.Bind();
	uniqueCube_.DrawInstanced(cubePositions_.size());

	blockManager_.Render();
	uiManager_.Render();
}

void PlayerProgram::Destroy()
{
	selectCube_.Destroy();
	uniqueCube_.Destroy();
	crossHair_.Destroy();
	shader_.Destroy();
	shaderLine_.Destroy();
	uiShader_.Destroy();

	cubePositions_.clear();
	cubeAabbs_.clear();
	gl::DestroyTexture(texture_);

	blockManager_.Destroy();
}

void PlayerProgram::DrawImGui()
{
	using namespace ImGui;
	Begin("Player variables");
	DragFloat3("Player Pos", &playerPos_.x, 0.0f, 0.0f, 0.0f, "%.7f");
	DragFloat("Gravity", &gravity_, 0.001f, 0.0f, 10.0f, "%.7f");
	DragFloat("Jump Power", &playerJumpPower_, 0.001f, 0.0f, 10.0f, "%.7f");
	DragFloat("Deceleration Speed", &decelerationSpeed_, 0.001f, 0.0f, 10.0f, "%.7f");
	DragFloat("Block Place Cooldown", &placeCoolDown_, 0.001f, 0.0f, 10.0f, "%.7f");
	Separator();
	Text("Time = %f", Time::time);
	Text("Fall Time = %f", timer_);
	Text("Test = %f", test_);
	End();
}

void PlayerProgram::OnEvent(const SDL_Event& event)
{
	camera_.OnEvent(event);
	
	if(event.window.event == SDL_WINDOWEVENT_RESIZED)
	{
		const Vec2f newWindowSire = Vec2f(event.window.data1, event.window.data2);
		{
			const Vec2f normalSpaceSize = Vec2f(toolBar_.size) / newWindowSire;
			toolBar_.position.y = normalSpaceSize.y / 2 - 1.0;
		}
		{
			const Vec2f tileSize = Vec2f(tileSize_) / newWindowSire;
			for (int i = 0; i < toolbarSize; ++i)
			{
				if (toolBarBlockIds_[i] == -1) continue;
				blockPreviews_[i].position.x = toolBar_.position.x + (i - 4) * tileSize.x;
				blockPreviews_[i].position.y = toolBar_.position.y;
			}
			
			blockSelect_.position.y = toolBar_.position.y;
			blockSelect_.position.x = toolBar_.position.x + (selectIndex_ - 4) * tileSize.x;
		}
	}
}

void PlayerProgram::CreateCube(const Vec3f& position)
{
	cubePositions_.emplace_back(position);
	cubeIds_.emplace_back(0);
	uniqueCube_.UpdateInstance(cubePositions_[0], cubePositions_.size());
	
	Aabb3d aabb;
	aabb.SetFromCenter(position, Vec3(cubeHalfSize));
	cubeAabbs_.emplace_back(aabb);
}

void PlayerProgram::PlaceCube(const Vec3f& position)
{
	Aabb3d aabb;
	aabb.SetFromCenter(position, Vec3(cubeHalfSize));
	if (aabb.IntersectAabb(playerAabb_)) return;

	placeTimeStamp_ = Time::time + placeCoolDown_;
	cubePositions_.emplace_back(position);
	cubeIds_.emplace_back(toolBarBlockIds_[selectIndex_]);
	uniqueCube_.UpdateInstance(cubePositions_[0], cubePositions_.size());
	
	cubeAabbs_.emplace_back(aabb);
}

void PlayerProgram::DeleteCube(const size_t& index)
{
	placeTimeStamp_ = Time::time + placeCoolDown_;
	cubePositions_.erase(cubePositions_.begin() + index);
	uniqueCube_.UpdateInstance(cubePositions_[0], cubePositions_.size());
	cubeAabbs_.erase(cubeAabbs_.begin() + index);
}

void PlayerProgram::MovePlayer()
{
	const auto& inputManager = static_cast<sdl::InputManager&>(sdl::InputLocator::get());
	if (inputManager.IsActionHeld(sdl::InputAction::RIGHT))
		playerVelocity_.x += Time::fixedDeltaTime * speedMultiplier_;
	if (inputManager.IsActionHeld(sdl::InputAction::LEFT))
		playerVelocity_.x -= Time::fixedDeltaTime * speedMultiplier_;

	if (inputManager.IsActionHeld(sdl::InputAction::UP))
		playerVelocity_.z += Time::fixedDeltaTime * speedMultiplier_;
	if (inputManager.IsActionHeld(sdl::InputAction::DOWN))
		playerVelocity_.z -= Time::fixedDeltaTime * speedMultiplier_;
	
	playerVelocity_.x = Lerp(playerVelocity_.x, 0.0f, decelerationSpeed_ * Time::fixedDeltaTime);
	playerVelocity_.z = Lerp(playerVelocity_.z, 0.0f, decelerationSpeed_ * Time::fixedDeltaTime);
	
	playerVelocity_.y -= gravity_ * Time::fixedDeltaTime;
	
	CheckPlayerPos();

	playerPos_ += camera_.GetRight() * playerVelocity_.x +
		Vec3f::up * playerVelocity_.y * Time::fixedDeltaTime - 
		Vec3f::Cross(camera_.GetRight(), Vec3f::up) * playerVelocity_.z;
	playerAabb_.SetFromCenter(playerPos_ + Vec3f::up, playerAabbSize_);
	
	camera_.position = playerPos_ + cameraOffset_;
}

void PlayerProgram::CheckPlayerPos()
{
	Ray rayOut;
	if (RayCast(rayOut, playerPos_ + Vec3f::up * 0.01f, Vec3f::down, maxReach_))
	{
		if (Equal(rayOut.hitDist, 0.01f, 0.001f)) 
			canJump_ = true;
		else
			canJump_ = false;
	}
	
	for (size_t i = 0; i < cubeAabbs_.size(); ++i)
	{
		const Vec3f fromCenter = cubePositions_[i] - Vec3f(playerPos_.x, cubePositions_[i].y - cubeHalfSize, playerPos_.z);
		if (cubeAabbs_[i].IntersectAabb(Aabb3d(playerPos_ + Vec3f(playerVelocity_.x, cubeHalfSize * 2, playerVelocity_.z), playerAabbSize_)) &&
			Abs(fromCenter.x) > Abs(fromCenter.y) && 
			Abs(fromCenter.x) > Abs(fromCenter.z))
		{
			const float sideSign = Sign(cubePositions_[i].x - playerPos_.x);
			playerPos_.x = cubePositions_[i].x - (cubeHalfSize + playerAabbSize_.x + 0.01f) * sideSign;
		}
		else if (cubeAabbs_[i].ContainsPoint(playerPos_ + Vec3f(0, playerVelocity_.y * Time::fixedDeltaTime, 0)) &&
			Abs(fromCenter.y) > Abs(fromCenter.x) && 
			Abs(fromCenter.y) > Abs(fromCenter.z))
		{
			const float sideSign = Sign(cubePositions_[i].y - playerPos_.y);
			playerPos_.y = cubePositions_[i].y - cubeHalfSize * sideSign;

			if (playerVelocity_.y <= 0.0f) playerVelocity_.y = 0.0f;
		}
		else if (cubeAabbs_[i].IntersectAabb(Aabb3d(playerPos_ + Vec3f(playerVelocity_.x, cubeHalfSize * 2, playerVelocity_.z), playerAabbSize_)) &&
			Abs(fromCenter.z) > Abs(fromCenter.y) && 
			Abs(fromCenter.z) > Abs(fromCenter.x))
		{
			const float sideSign = Sign(cubePositions_[i].z - playerPos_.z);
			playerPos_.z = cubePositions_[i].z - (cubeHalfSize + playerAabbSize_.z + 0.01f) * sideSign;
		}
	}
}

void PlayerProgram::HeadBobbing()
{
	if (IsMoving())
    {
        cameraOffset_ = camera_.GetRight() * std::sin(bobbingTimer_ / 2) * bobbingAmount_;
        cameraOffset_.y = defaultPosY_ + std::sin(bobbingTimer_) * bobbingAmount_;
        bobbingTimer_ += walkingBobbingSpeed_ * speedMultiplier_ * Time::deltaTime;
    }
    else
    {
        bobbingTimer_ = 0;
        cameraOffset_.x = Lerp(cameraOffset_.x, 0.0f, walkingBobbingSpeed_ * speedMultiplier_ * Time::deltaTime);
        cameraOffset_.y = Lerp(cameraOffset_.y, defaultPosY_, walkingBobbingSpeed_ * speedMultiplier_ * Time::deltaTime);
        cameraOffset_.z = Lerp(cameraOffset_.z, 0.0f, walkingBobbingSpeed_ * speedMultiplier_ * Time::deltaTime);

    	if (Equal(cameraOffset_.x, 0.0f, 0.0001f)) cameraOffset_.x = 0.0f;
    	if (Equal(cameraOffset_.z, 0.0f, 0.0001f)) cameraOffset_.z = 0.0f;
    }
}

void PlayerProgram::CheckBlock()
{
	const auto& inputManager = static_cast<sdl::InputManager&>(sdl::InputLocator::get());
	
	Ray rayOut;
	const Vec3f direction = camera_.reverseDirection.Normalized() * -1;
	if (RayCast(rayOut, camera_.position, direction, maxReach_))
	{
		shaderLine_.Bind();
		Mat4f model = Mat4f::Identity;
		model = Transform3d::Translate(model, cubePositions_[rayOut.hitIndex]);
		
		shaderLine_.SetMat4("projection", camera_.GenerateProjectionMatrix());
		shaderLine_.SetMat4("view", camera_.GenerateViewMatrix());
		shaderLine_.SetMat4("model", model);
		shaderLine_.SetVec3("color", Vec3f::zero);
		glLineWidth(3.0f);
		selectCube_.Draw();
		
		if (Time::time < placeTimeStamp_) return;
		if (inputManager.IsMouseButtonHeld(sdl::MouseButtonCode::LEFT))
		{
			DeleteCube(rayOut.hitIndex);
		}
		if (inputManager.IsMouseButtonHeld(sdl::MouseButtonCode::RIGHT) && toolBarBlockIds_[selectIndex_] != -1)
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
	}
}

bool PlayerProgram::IsMoving()
{
	const auto& inputManager = static_cast<sdl::InputManager&>(sdl::InputLocator::get());
	return inputManager.IsActionHeld(sdl::InputAction::RIGHT) ||
		inputManager.IsActionHeld(sdl::InputAction::LEFT) ||
		inputManager.IsActionHeld(sdl::InputAction::UP) ||
		inputManager.IsActionHeld(sdl::InputAction::DOWN);
}

bool PlayerProgram::RayCast(
    Ray& out,
	const Vec3f& origin, 
	const Vec3f& direction,
	const float distance)
{
	float closestDist = distance * distance;
	for (size_t i = 0; i < cubeAabbs_.size(); ++i)
	{
		float minDist;
		if (cubeAabbs_[i].IntersectRay(direction, origin, minDist))
		{
			const float dist = (cubePositions_[i] - origin).SquareMagnitude();
			if (dist < closestDist)
			{
				closestDist = dist;
				out.hitDist = minDist;
				out.hitPos = cubePositions_[i];
				out.hitIndex = i;
			}
		}
	}
	return out.hitIndex != -1;
}
}
