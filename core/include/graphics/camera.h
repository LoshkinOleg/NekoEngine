#pragma once

#include <mathematics/vector.h>
#include <mathematics/matrix.h>
#include <mathematics/transform.h>

namespace neko
{
struct Camera
{
    Vec3f position;
    Vec3f reverseDirection;

	Vec3f GetRight() const
	{
		return Vec3f::Cross(Vec3f::up, reverseDirection).Normalized();
	}

	Vec3f GetUp() const
	{
		const Vec3f right = GetRight();
		return Vec3f::Cross(reverseDirection, right);
	}
	
	Mat4f GenerateViewMatrix() const
	{
		const Vec3f right = GetRight();
		const Vec3f up = GetUp();
		const Mat4f rotation(std::array<Vec4f, 4>{
			Vec4f(right.x, up.x, reverseDirection.x, 0.0f),
				Vec4f(right.y, up.y, reverseDirection.y, 0.0f),
				Vec4f(right.z, up.z, reverseDirection.z, 0.0f),
				Vec4f(0.0f, 0.0f, 0.0f, 1.0f)
		});
		const Mat4f translation(std::array<Vec4f, 4>{
			Vec4f(1,0,0,0),
			Vec4f(0,1,0,0),
			Vec4f(0,0,1,0),
			Vec4f(-position.x,-position.y,-position.z,1),
		});
		return rotation * translation;
	}

	void SetDirectionFromEuler(const EulerAngles& angles)
	{
		const Quaternion q = Quaternion::FromEuler(angles);
		reverseDirection = Vec3f(Transform3d::RotationMatrixFrom(q)*Vec4f(0,0,1,0));
	}
	
	void Rotate(const EulerAngles& angles)
	{
		const auto pitch = Quaternion::AngleAxis(angles.x, GetRight());

		const auto yaw = Quaternion::AngleAxis(angles.y, GetUp());

		const auto roll = Quaternion::AngleAxis(angles.z, reverseDirection);
		reverseDirection = Vec3f(Transform3d::RotationMatrixFrom(pitch*yaw*roll) * Vec4f(reverseDirection));
	}
};

struct Camera3D : Camera
{
	float aspect = 1.0f;
	degree_t fovY = degree_t(45.0f);
	float nearPlane = 0.1f;
	float farPlane = 100.0f;
	Mat4f GenerateProjectionMatrix() const
	{
		return Transform3d::Perspective(
			fovY,
			aspect,
			nearPlane,
			farPlane);
	};

	void SetAspect(const int width, const int height)
	{
		aspect = static_cast<float>(width) / static_cast<float>(height);
	}
};

struct MoveableCamera3D final : Camera3D, sdl::SdlEventSystemInterface, SystemInterface
{
	void Init() override
	{
		position = kCameraOriginPos_;
		reverseDirection = Vec3f::forward;
	}
	
	void Update(seconds dt) override
	{
		//Retrieve the input manager
		const auto& inputManager = static_cast<sdl::InputManager&>(sdl::InputLocator::get());

		//Check if left click is pressed
		if (inputManager.IsMouseButtonHeld(sdl::MouseButtonCode::RIGHT))
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
		if (inputManager.IsActionHeld(sdl::InputAction::RIGHT))
			cameraMove.x += dt.count();
		if (inputManager.IsActionHeld(sdl::InputAction::LEFT))
			cameraMove.x -= dt.count();
		if (inputManager.IsActionHeld(sdl::InputAction::JUMP))
			cameraMove.y += dt.count();
		if (inputManager.IsActionHeld(sdl::InputAction::CROUCH))
			cameraMove.y -= dt.count();
		if (inputManager.IsActionHeld(sdl::InputAction::UP))
			cameraMove.z += dt.count();
		if (inputManager.IsActionHeld(sdl::InputAction::DOWN))
			cameraMove.z -= dt.count();
		
		//Boost key test
		if (inputManager.IsActionHeld(sdl::InputAction::ZOOM))
			cameraMove *= 3.0f;
		
		//Apply camera movement
		position += (GetRight() * cameraMove.x + 
			Vec3f::up * cameraMove.y - 
			reverseDirection * cameraMove.z) * moveSpeed_;
	}

	void OnEvent(const SDL_Event& event) override
	{
		//Retrieves the amount of mouse movement between each frame
		if (event.type == SDL_MOUSEMOTION)
			mouseMotion_ = Vec2f(-event.motion.xrel, -event.motion.yrel) * mouseSpeed_;

		if (event.type == SDL_WINDOWEVENT_RESIZED)
		{
			const auto& config = BasicEngine::GetInstance()->config;
			SetAspect(config.windowSize.x, config.windowSize.y);
		}
	}

	void Destroy() override
	{
		
	}
private:
	const Vec3f kCameraOriginPos_ = Vec3f(0.0f, 0.0f, -10.0f);
	const EulerAngles kCameraOriginAngles_ = EulerAngles(
		degree_t(0.0f), degree_t(0.0f), degree_t(0.0f));
	Vec2f mouseMotion_ = Vec2f::zero;

	float moveSpeed_ = 3.0f;
	float mouseSpeed_ = 0.1f;

};
}
