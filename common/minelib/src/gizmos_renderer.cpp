#include <minelib/gizmos_renderer.h>

#include <engine/engine.h>

namespace neko
{
GizmosRenderer::GizmosRenderer(FpsCamera* camera)
	: camera_(camera)
{
	GizmosLocator::provide(this);
}

void GizmosRenderer::Init()
{
	const auto& config = BasicEngine::GetInstance()->config;
	shaderCube_.LoadFromFile(
		config.dataRootPath + "shaders/minecraft_like/gizmoCube.vert",
		config.dataRootPath + "shaders/minecraft_like/gizmoCube.frag");
	shaderLine_.LoadFromFile(
		config.dataRootPath + "shaders/minecraft_like/gizmoLine.vert",
		config.dataRootPath + "shaders/minecraft_like/gizmoLine.frag");
	cube_.Init();
	line_.Init();
	gizmosQueue_.reserve(kGizmoReserveSize);
}

void GizmosRenderer::Update(seconds dt)
{
}

void GizmosRenderer::Render()
{
	if (isRunning_)
	{
		for (auto gizmo : gizmosQueue_)
		{
			if (shaderCube_.GetProgram() == 0) continue;

			std::lock_guard<std::mutex> lock(renderMutex_);
			switch (gizmo.shape)
			{
			case GizmoShape::CUBE:
			{
				shaderCube_.Bind();
				shaderCube_.SetMat4("view", camera_->GenerateViewMatrix());
				shaderCube_.SetMat4("projection", camera_->GenerateProjectionMatrix());
				shaderCube_.SetVec4("color", gizmo.color);
				Mat4f model = Mat4f::Identity;
				model = Transform3d::Scale(model, gizmo.cubeSize);
				model = Transform3d::Translate(model, gizmo.pos);
				shaderCube_.SetMat4("model", model);

				glLineWidth(gizmo.lineThickness);
				cube_.Draw();
			}
			break;
			case GizmoShape::LINE:
			{
				shaderLine_.Bind();
				shaderLine_.SetMat4("view", camera_->GenerateViewMatrix());
				shaderLine_.SetMat4("projection", camera_->GenerateProjectionMatrix());
				shaderCube_.SetVec4("color", gizmo.color);
				Mat4f model = Transform3d::Translate(Mat4f::Identity, gizmo.pos);
				shaderLine_.SetMat4("model", model);
				shaderLine_.SetVec3("endPos", gizmo.lineEndPos - gizmo.pos);
				
				glLineWidth(gizmo.lineThickness);
				line_.Draw();
			}
			break;
			default:
				LogError("Invalid Gizmo shape!");
			}
		}
		gizmosQueue_.clear();
		gizmosQueue_.reserve(kGizmoReserveSize);
	}
}

void GizmosRenderer::Destroy()
{
	cube_.Destroy();
	line_.Destroy();
	shaderCube_.Destroy();
}

void GizmosRenderer::Start()
{
	isRunning_ = true;
}

void GizmosRenderer::Stop()
{
	isRunning_ = false;
}

void GizmosRenderer::DrawCube(
	const Vec3f& pos,
	const Vec3f& size,
	const Color4& color,
	const float lineThickness)
{
	if (isRunning_)
	{
		std::lock_guard<std::mutex> lock(renderMutex_);
		Gizmos gizmo;
		gizmo.pos = pos;
		gizmo.cubeSize = size;
		gizmo.color = color;
		gizmo.shape = GizmoShape::CUBE;
		gizmosQueue_.push_back(gizmo);
	}
}

void GizmosRenderer::DrawLine(
	const Vec3f& startPos,
	const Vec3f& endPos,
	const Color4& color,
	const float lineThickness)
{
	if (isRunning_)
	{
		std::lock_guard<std::mutex> lock(renderMutex_);
		Gizmos gizmo;
		gizmo.pos = startPos;
		gizmo.lineEndPos = endPos;
		gizmo.color = color;
		gizmo.shape = GizmoShape::LINE;
		gizmosQueue_.push_back(gizmo);
	}
}

void GizmosRenderer::SetCamera(FpsCamera* camera)
{
	camera_ = camera;
}
}
