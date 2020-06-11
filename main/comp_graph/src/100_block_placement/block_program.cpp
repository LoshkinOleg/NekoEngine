#include <100_block_placement/block_program.h>

#include <engine/engine.h>
#include <gl/texture.h>

namespace neko
{
void BlockProgram::Init()
{
	const auto& config = BasicEngine::GetInstance()->config;
	shader_.LoadFromFile(
		config.dataRootPath + "shaders/base.vert",
		config.dataRootPath + "shaders/base.frag");
	texture_ = gl::stbCreateTexture(config.dataRootPath + "sprites/wall.jpg");
	cube_.Init();

	camera_.Init();
	camera_.position = Vec3f::back * 3;

	CreateCube(Vec3f::zero);
	CreateCube(Vec3f::right);
	CreateCube(Vec3f::left);
	CreateCube(Vec3f::one);
	CreateCube(Vec3f(-3.0f, 5.0f, 0.0f));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void BlockProgram::Update(seconds dt)
{
	std::lock_guard<std::mutex> lock(updateMutex_);

	camera_.Update(dt);
}

void BlockProgram::Render()
{
	if (shader_.GetProgram() == 0) return;
	if (texture_ == INVALID_TEXTURE_ID) return;

	std::lock_guard<std::mutex> lock(updateMutex_);
	const auto& inputManager = static_cast<sdl::InputManager&>(sdl::InputLocator::get());

	shader_.Bind();
	glBindTexture(GL_TEXTURE_2D, texture_); //bind texture id to texture slot
	shader_.SetMat4("view", camera_.GenerateViewMatrix());
	shader_.SetMat4("projection", camera_.GenerateProjectionMatrix());

	Ray rayOut;
	const Vec3f direction = camera_.reverseDirection.Normalized() * -1;
	if (RayCast(rayOut, camera_.position, direction, maxReach_))
	{
		if (inputManager.IsMouseButtonDown(sdl::MouseButtonCode::LEFT))
		{
			DeleteCube(rayOut.hitIndex);
		}
		if (inputManager.IsMouseButtonDown(sdl::MouseButtonCode::RIGHT))
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

			Aabb3d hitTest;
			hitTest.SetFromCenter(camera_.position, Vec3f(0.5f));
			if (!hitTest.ContainsPoint(rayOut.hitPos - offset))
			{
				CreateCube(rayOut.hitPos - offset);
				
				std::cout << "Point Pos: " << cubePoint << '\n';
				std::cout << "Relative: " << fromCenter << '\n';
			}
		}
	}
	
	for (auto& cubePosition : cubePositions_)
	{
		Mat4f model = Mat4f::Identity;
		model = Transform3d::Translate(model, cubePosition);
		
		shader_.SetMat4("model", model);
		cube_.Draw();
	}
}

void BlockProgram::Destroy()
{
	cube_.Destroy();
	shader_.Destroy();
	gl::DestroyTexture(texture_);
}

void BlockProgram::DrawImGui()
{
}

void BlockProgram::OnEvent(const SDL_Event& event)
{
	camera_.OnEvent(event);
}

void BlockProgram::CreateCube(const Vec3f& position)
{
	cubePositions_.emplace_back(position);
	
	Aabb3d aabb;
	aabb.SetFromCenter(position, Vec3(0.5f));
	cubeAabbs_.emplace_back(aabb);
}

void BlockProgram::DeleteCube(const size_t& index)
{
	cubePositions_.erase(cubePositions_.begin() + index);
	cubeAabbs_.erase(cubeAabbs_.begin() + index);
}

bool BlockProgram::RayCast(
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
