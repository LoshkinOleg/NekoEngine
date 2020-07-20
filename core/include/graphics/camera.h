#pragma once
#include <engine/engine.h>
#include <sdl_engine/sdl_input.h>
#include <sdl_engine/sdl_engine.h>

#include <mathematics/vector.h>
#include <mathematics/matrix.h>
#include <mathematics/transform.h>

namespace neko
{
struct Camera
{
    Vec3f position = Vec3f::back;
    Vec3f reverseDirection = Vec3f::back;
	
	float nearPlane = 0.1f;
	float farPlane = 10'000.0f;

	void WorldLookAt(const Vec3f& target)
	{
		const Vec3f direction = position - target;
		reverseDirection = direction.Normalized();
	}
	
	[[nodiscard]] Vec3f GetRight() const
	{
		if (reverseDirection.x == 0.0f && reverseDirection.z == 0.0f)
			return Vec3f::Cross(Vec3f::forward, reverseDirection).Normalized();
		return Vec3f::Cross(Vec3f::up, reverseDirection).Normalized();
	}

	[[nodiscard]] Vec3f GetUp() const
	{
		const Vec3f right = GetRight();
		return Vec3f::Cross(reverseDirection, right).Normalized();
	}

	EulerAngles GetRotation() const
	{
		const auto inverseView = GenerateViewMatrix().Inverse();
		return EulerAngles(
			Atan2(inverseView[1][2], inverseView[2][2]),
			Atan2(-inverseView[0][2], sqrtf(inverseView[1][2] * inverseView[1][2] + 
				inverseView[2][2] * inverseView[2][2])),
			Atan2(inverseView[0][1], inverseView[0][0]));
	}

	Mat4f GetRotationMat() const
	{
		const Vec3f right = GetRight();
		const Vec3f up = GetUp();
		return Mat4f(std::array<Vec4f, 4>{
				Vec4f(right.x, up.x, reverseDirection.x, 0),
				Vec4f(right.y, up.y, reverseDirection.y, 0),
				Vec4f(right.z, up.z, reverseDirection.z, 0),
				Vec4f(0, 0, 0, 1)
		});
	}
	
	[[nodiscard]] Mat4f GenerateViewMatrix() const
	{
		const Mat4f rotation = GetRotationMat();
		const Mat4f translation =
				Transform3d::Translate(Mat4f::Identity, position * -1);
		return rotation * translation;
	}

	void SetDirectionFromEuler(const EulerAngles& angles)
	{
		const Quaternion q = Quaternion::FromEuler(angles);
		reverseDirection = Vec3f(Transform3d::RotationMatrixFrom(q)*Vec4f(0,0,1,0));
	}
	
	void Rotate(const EulerAngles& angles)
	{
		const auto camRot = GetRotation();
		
		Quaternion pitch;
		if (Abs(camRot.x.value()) > 89.0f && Sign(reverseDirection.z) == 1 ||
			Abs(camRot.x.value()) < 91.0f && Sign(reverseDirection.z) != 1) 
			pitch = Quaternion::Identity;
		else
			pitch = Quaternion::AngleAxis(angles.x, GetRight());

		const auto yaw = Quaternion::AngleAxis(angles.y, GetUp());

		const auto roll = Quaternion::AngleAxis(angles.z, reverseDirection);

		const Quaternion rotation = pitch * yaw * roll;
		reverseDirection = Vec3f(Transform3d::RotationMatrixFrom(rotation) * Vec4f(reverseDirection));
	}
	
	float GetAspect() const { return aspect_; }
	
	void SetAspect(const int width, const int height)
	{
		aspect_ = static_cast<float>(width) / static_cast<float>(height);
	}

	virtual Mat4f GenerateProjectionMatrix() const = 0;
protected:
	float aspect_ = 1.0f;
};

struct Camera2D : Camera
{
	float right = 0.0f, left = 0.0f, top = 0.0f, bottom =0.0f;
	[[nodiscard]] Mat4f GenerateProjectionMatrix() const override
	{
		return Mat4f(std::array<Vec4f, 4>{
			Vec4f(2.0f / (right - left), 0, 0, 0),
			Vec4f(0, 2.0f / (top - bottom), 0, 0),
			Vec4f(0, 0, -2.0f / (farPlane - nearPlane), 0),
			Vec4f(-(right + left) / (right - left), 
					-(top + bottom) / (top - bottom), 
					-(farPlane + nearPlane) / (farPlane - nearPlane), 1.0f)
		});
	}
	void SetSize(Vec2f size)
	{
		left = -size.x;
		right = size.x;
		top = size.y;
		bottom = -size.y;
	}
};

struct Camera3D : Camera
{
	degree_t fovY = degree_t(45.0f);
	[[nodiscard]] Mat4f GenerateProjectionMatrix() const override
	{
		return Transform3d::Perspective(
				fovY,
				aspect_,
				nearPlane,
				farPlane);
	}

	radian_t GetFovX() const
	{
		return 2.0f*Atan(Tan(fovY*0.5f) * aspect_);
	}
};

struct MovableCamera : sdl::SdlEventSystemInterface, SystemInterface
{
	float moveSpeed = 2.5f;
	float mouseSpeed = 0.1f;

	MovableCamera() :
		inputManager_(static_cast<sdl::InputManager&>(sdl::InputLocator::get()))
	{
	}
	
	MovableCamera& operator=(const MovableCamera& other)
	{
		mouseSpeed = other.mouseSpeed;
		moveSpeed = other.moveSpeed;
	
		return *this;
	}
protected:
	Vec2f mouseMotion_;
	sdl::InputManager& inputManager_;
};

struct MoveableCamera2D final : Camera2D, MovableCamera
{
	void Init() override
	{
	}
	
	void Update(const seconds dt) override
	{
		//Check if left click is pressed
		if (inputManager_.IsMouseButtonHeld(sdl::MouseButtonCode::RIGHT))
		{
			Rotate(EulerAngles(
					degree_t(mouseMotion_.y),
					degree_t(mouseMotion_.x),
					degree_t(0.0f)
			));
			mouseMotion_ = Vec2f::zero;
		}
		
		//Movement keys tests
		Vec3f cameraMove = Vec3f();
		if (inputManager_.IsActionHeld(sdl::InputAction::RIGHT))
			cameraMove.x += dt.count();
		if (inputManager_.IsActionHeld(sdl::InputAction::LEFT))
			cameraMove.x -= dt.count();
		if (inputManager_.IsActionHeld(sdl::InputAction::JUMP))
			cameraMove.y += dt.count();
		if (inputManager_.IsActionHeld(sdl::InputAction::CROUCH))
			cameraMove.y -= dt.count();
		if (inputManager_.IsActionHeld(sdl::InputAction::UP))
			cameraMove.z += dt.count();
		if (inputManager_.IsActionHeld(sdl::InputAction::DOWN))
			cameraMove.z -= dt.count();
		
		//Boost key test
		if (inputManager_.IsActionHeld(sdl::InputAction::SPRINT))
			cameraMove *= 3.0f;
		
		//Apply camera movement
		position += (GetRight() * cameraMove.x + 
			Vec3f::up * cameraMove.y - 
			reverseDirection * cameraMove.z) * moveSpeed;
	}

	void OnEvent(const SDL_Event& event) override
	{
		if(event.window.event == SDL_WINDOWEVENT_RESIZED)
		{
			const auto& config = BasicEngine::GetInstance()->config;
			SetAspect(config.windowSize.x, config.windowSize.y);
		}
		
		//Retrieves the amount of mouse movement between each frame
		if (event.type == SDL_MOUSEMOTION)
			mouseMotion_ = Vec2f(-event.motion.xrel, -event.motion.yrel) * mouseSpeed;
	}

	void Destroy() override
	{
	}
};

struct MoveableCamera3D : Camera3D, public MovableCamera
{
	void Init() override
	{
	}
	
	void Update(const seconds dt) override
	{
		//Check if left click is pressed
		if (inputManager_.IsMouseButtonHeld(sdl::MouseButtonCode::RIGHT))
		{
			Rotate(EulerAngles(
					degree_t(mouseMotion_.y),
					degree_t(mouseMotion_.x),
					degree_t(0.0f)
			));
			mouseMotion_ = Vec2f::zero;
		}
		
		//Movement keys tests
		Vec3f cameraMove = Vec3f();
		if (inputManager_.IsActionHeld(sdl::InputAction::RIGHT))
			cameraMove.x += dt.count();
		if (inputManager_.IsActionHeld(sdl::InputAction::LEFT))
			cameraMove.x -= dt.count();
		if (inputManager_.IsActionHeld(sdl::InputAction::JUMP))
			cameraMove.y += dt.count();
		if (inputManager_.IsActionHeld(sdl::InputAction::CROUCH))
			cameraMove.y -= dt.count();
		if (inputManager_.IsActionHeld(sdl::InputAction::UP))
			cameraMove.z += dt.count();
		if (inputManager_.IsActionHeld(sdl::InputAction::DOWN))
			cameraMove.z -= dt.count();
		
		//Boost key test
		if (inputManager_.IsActionHeld(sdl::InputAction::SPRINT))
			cameraMove *= 3.0f;
		
		//Apply camera movement
		position += (GetRight() * cameraMove.x + 
			Vec3f::up * cameraMove.y - 
			reverseDirection * cameraMove.z) * moveSpeed;
	}
	
	void FixedUpdate() override {}

	void OnEvent(const SDL_Event& event) override
	{
		if(event.window.event == SDL_WINDOWEVENT_RESIZED)
		{
			const auto& config = BasicEngine::GetInstance()->config;
			SetAspect(config.windowSize.x, config.windowSize.y);
		}
		
		//Retrieves the amount of mouse movement between each frame
		if (event.type == SDL_MOUSEMOTION)
			mouseMotion_ = Vec2f(-event.motion.xrel, -event.motion.yrel) * mouseSpeed;
	}

	void Destroy() override
	{}
};

struct FpsCamera final : public MoveableCamera3D
{
	bool freezeCam = false;

	FpsCamera& operator=(const FpsCamera& other)
	{
		position = other.position;
		reverseDirection = other.reverseDirection;
		
		farPlane = other.farPlane;
		nearPlane = other.nearPlane;
		fovY = other.fovY;
		
		mouseSpeed = other.mouseSpeed;
		moveSpeed = other.moveSpeed;
		mouseMotion_ = other.mouseMotion_;
		
		freezeCam = other.freezeCam;
	
		return *this;
	}

	void Init() override
	{
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}

	void Update(const seconds dt) override
	{
		//Check if left click is pressed
		if (!freezeCam)
		{
			Rotate(EulerAngles(
					degree_t(mouseMotion_.y),
					degree_t(mouseMotion_.x),
					degree_t(0.0f)
			));
			mouseMotion_ = Vec2f::zero;
		}
	}

	void FixedUpdate() override
	{
		if (!freezeCam)
		{	
			//Checking if keys are down
			Vec3f cameraMove = Vec3f();
			if (inputManager_.IsActionHeld(sdl::InputAction::RIGHT))
				cameraMove.x += Time::fixedDeltaTime;
			if (inputManager_.IsActionHeld(sdl::InputAction::LEFT))
				cameraMove.x -= Time::fixedDeltaTime;
			if (inputManager_.IsActionHeld(sdl::InputAction::JUMP))
				cameraMove.y += Time::fixedDeltaTime;
			if (inputManager_.IsActionHeld(sdl::InputAction::CROUCH))
				cameraMove.y -= Time::fixedDeltaTime;
			if (inputManager_.IsActionHeld(sdl::InputAction::UP))
				cameraMove.z += Time::fixedDeltaTime;
			if (inputManager_.IsActionHeld(sdl::InputAction::DOWN))
				cameraMove.z -= Time::fixedDeltaTime;

			//Boost key test
			if (inputManager_.IsActionHeld(sdl::InputAction::SPRINT))
				cameraMove *= 3.0f;
			
			//Apply camera movement
			position += (GetRight() * cameraMove.x + 
				Vec3f::up * cameraMove.y - 
				Vec3f::Cross(GetRight(), Vec3f::up) * cameraMove.z) * moveSpeed;
		}
	}

	void OnEvent(const SDL_Event& event) override
	{
		if (inputManager_.IsActionDown(sdl::InputAction::MENU))
		{
			freezeCam = !freezeCam;
			SDL_SetRelativeMouseMode(static_cast<SDL_bool>(!freezeCam));
		}
		
		if(event.window.event == SDL_WINDOWEVENT_RESIZED)
		{
			SetAspect(event.window.data1, event.window.data2);
		}
		
		if (!freezeCam)
		{
			if (event.type == SDL_MOUSEMOTION)
				mouseMotion_ = Vec2f(-event.motion.xrel, -event.motion.yrel) * mouseSpeed;
			
			SDL_WarpMouseGlobal(event.window.data1 / 2, event.window.data2 / 2);
		}
	}

	void Destroy() override
	{
	}
};

struct PlayerCamera final : public MoveableCamera3D
{
	bool freezeCam = false;

	void Init() override
	{
		freezeCam = false;
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}

	void Update(const seconds dt) override
	{
	}

	void FixedUpdate() override
	{
	}

	void OnEvent(const SDL_Event& event) override
	{
		if (inputManager_.IsActionDown(sdl::InputAction::MENU))
		{
			freezeCam = !freezeCam;
			SDL_SetRelativeMouseMode(static_cast<SDL_bool>(!freezeCam));
		}
		
		if(event.window.event == SDL_WINDOWEVENT_RESIZED)
		{
			SetAspect(event.window.data1, event.window.data2);
		}
		
		if (!freezeCam)
		{
			if (event.type == SDL_MOUSEMOTION)
			{
				Rotate(EulerAngles(
						degree_t(-event.motion.yrel) * mouseSpeed,
						degree_t(-event.motion.xrel) * mouseSpeed,
						degree_t(0.0f)
				));
			}
			
			SDL_WarpMouseGlobal(event.window.data1 / 2, event.window.data2 / 2);
		}
	}

	void Destroy() override
	{
	}
};
}
