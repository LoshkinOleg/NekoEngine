#include "05_hello_camera/camera_program.h"

#include <sstream>

#include "imgui.h"

namespace neko
{
void HelloCameraProgram::Init()
{
	const auto& config = BasicEngine::GetInstance()->config;
    shader_.LoadFromFile(
            config.dataRootPath + "shaders/04_hello_coords/coords.vert",
            config.dataRootPath + "shaders/04_hello_coords/coords.frag");
	textureWall_ = gl::stbCreateTexture(config.dataRootPath + "sprites/wall.jpg");

	cube_.Init();
	
	camera_.position = cameraOriginPos;
	camera_.SetDirectionFromEuler(cameraOriginAngles);

}

void HelloCameraProgram::Update(seconds dt)
{
	std::lock_guard<std::mutex> lock(updateMutex_);
	
	const auto& config = BasicEngine::GetInstance()->config;
	projection_ = Transform3d::Perspective(
		degree_t(45.0f),
		static_cast<float>(config.windowSize.x) / config.windowSize.y,
		0.1f,
		100.0f);
	if (!(cameraMovement_ & DISABLE))
	{
		
		//Checking if mouse right button is down
		const auto mouseState = SDL_GetMouseState(nullptr, nullptr);
		cameraMovement_ = mouseState & SDL_BUTTON(3) ?
			cameraMovement_ | MOUSE_MOVE :
			cameraMovement_ & ~MOUSE_MOVE;
		
		if(cameraMovement_ & MOUSE_MOVE && mouseMotion_.SquareMagnitude() > 0.001f)
		{
			camera_.Rotate(EulerAngles(
				degree_t(mouseMotion_.y),
				degree_t(mouseMotion_.x),
				degree_t(0.0f)
			));
			mouseMotion_ = Vec2f::zero;
		}
		
		//Checking if keys are down
		const Uint8* keys = SDL_GetKeyboardState(NULL);
		cameraMovement_ =
			keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A] ?
			cameraMovement_ | LEFT :
			cameraMovement_ & ~LEFT;
		cameraMovement_ =
			keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D] ?
			cameraMovement_ | RIGHT :
			cameraMovement_ & ~RIGHT;
		cameraMovement_ =
			keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W] ?
			cameraMovement_ | UP :
			cameraMovement_ & ~UP;
		cameraMovement_ =
			keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S] ?
			cameraMovement_ | DOWN :
			cameraMovement_ & ~DOWN;
		cameraMovement_ =
			keys[SDL_SCANCODE_LSHIFT] ?
			cameraMovement_ | ACCELERATE :
			cameraMovement_ & ~ACCELERATE;
		Vec2f cameraMove = Vec2f();
		if (cameraMovement_ & LEFT)
		{
			cameraMove.x -= 1.0f * dt.count();
		}
		if (cameraMovement_ & RIGHT)
		{
			cameraMove.x += 1.0f * dt.count();
		}
		if (cameraMovement_ & UP)
		{
			cameraMove.y += 1.0f * dt.count();
		}
		if (cameraMovement_ & DOWN)
		{
			cameraMove.y -= 1.0f * dt.count();
		}
		camera_.position += 
			(camera_.rightDir * cameraMove.x -
			camera_.reverseDir * cameraMove.y)*
			(cameraMovement_ & ACCELERATE ? cameraFast_ : cameraSpeed_);
	}
}

void HelloCameraProgram::Destroy()
{
	shader_.Destroy();
	cube_.Destroy();
	gl::DestroyTexture(textureWall_);
}

void HelloCameraProgram::DrawImGui()
{
	ImGui::Begin("Camera Controls");
	ImGui::InputFloat3("Position", &camera_.position[0]);
	ImGui::InputFloat3("Direction", &camera_.reverseDir[0]);
	Vec3f eulerAngles = Vec3f(cameraAngles.x.value(), cameraAngles.y.value(), cameraAngles.z.value());
	if(ImGui::InputFloat3("Euler Angles", &eulerAngles[0]))
	{
		cameraAngles = EulerAngles(
			degree_t(eulerAngles.x),
			degree_t(eulerAngles.y),
			degree_t(eulerAngles.z)
		);
		camera_.SetDirectionFromEuler(cameraAngles);
	}
	if(ImGui::Button("Reset"))
	{
		camera_.position = cameraOriginPos;
		camera_.SetDirectionFromEuler(cameraOriginAngles);
	}

	bool cameraControlsDisabled = (cameraMovement_ & DISABLE);
	if(ImGui::Checkbox("Disable Controls", & cameraControlsDisabled))
	{
		cameraMovement_ = cameraControlsDisabled ? 
			cameraMovement_ | DISABLE :
			cameraMovement_ & ~DISABLE;
	}
	ImGui::End();
	
}

void HelloCameraProgram::Render()
{
	if (shader_.GetProgram() == 0)
		return;

	std::lock_guard<std::mutex> lock(updateMutex_);
	shader_.Bind();
	glBindTexture(GL_TEXTURE_2D, textureWall_);
	shader_.SetMat4("view", camera_.GenerateViewMatrix());
	shader_.SetMat4("projection", projection_);

	
	for(int i = -cubeNumbers_/2; i <= cubeNumbers_/2; i++)
	{
		Mat4f model = Mat4f::Identity; //model transform matrix
		Vec3f cubePosition(cubeDistance_ * i, 0, cubeDistance_*i);
		model = Transform3d::Translate(model, cubePosition);
		shader_.SetMat4("model", model);
		cube_.Draw();

		model = Mat4f::Identity;
		cubePosition = Vec3f(cubeDistance_ * i, 0, -cubeDistance_ * i);
		model = Transform3d::Translate(model, cubePosition);
		shader_.SetMat4("model", model);
		cube_.Draw();
	}
}

void HelloCameraProgram::OnEvent(const SDL_Event& event)
{
	switch(event.type)
	{

	case SDL_MOUSEMOTION:
	{
		mouseMotion_ = Vec2f(event.motion.xrel, event.motion.yrel)/mouseMotionRatio_;
		std::ostringstream oss;
		oss << "Mouse motion: " << mouseMotion_;
		LogDebug(oss.str());
			
		break;
	}
	case SDL_FINGERDOWN:
		break;
	case SDL_FINGERUP:
		break;
	case SDL_FINGERMOTION:
		break;
	default:
		break;
	}
}
}
