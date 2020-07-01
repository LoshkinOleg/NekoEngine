#include <gizmos_renderer.h>

#include <engine/engine.h>

namespace neko
{
GizmosRenderer::GizmosRenderer(Camera3D& camera) : camera_(camera)
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
	std::lock_guard<std::mutex> lock(updateMutex_);
}

void GizmosRenderer::Render()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	for (auto gizmo : gizmosQueue_)
	{
		if (gizmo.shape == GizmoShape::CUBE)
		{
			if (shaderCube_.GetProgram() == 0) continue;

			std::lock_guard<std::mutex> lock(updateMutex_);

			shaderCube_.Bind();
			shaderCube_.SetMat4("view", camera_.GenerateViewMatrix());
			shaderCube_.SetMat4("projection", camera_.GenerateProjectionMatrix());

			Mat4f model = Mat4f::Identity; //model transform matrix
			model = Transform3d::Scale(model, gizmo.cubeSize);
			model = Transform3d::Translate(model, gizmo.pos);
			shaderCube_.SetMat4("model", model);
			shaderCube_.SetVec4("color", gizmo.color);
			cube_.Draw();
		}

		if (gizmo.shape == GizmoShape::LINE)
		{
			if (shaderCube_.GetProgram() == 0) continue;

			std::lock_guard<std::mutex> lock(updateMutex_);

			shaderLine_.Bind();
			shaderLine_.SetMat4("view", camera_.GenerateViewMatrix());
			shaderLine_.SetMat4("projection", camera_.GenerateProjectionMatrix());
			
			Mat4f model = Transform3d::Translate(Mat4f::Identity, gizmo.pos);
			shaderLine_.SetMat4("model", model);
			shaderLine_.SetVec4("color", gizmo.color);
			shaderLine_.SetVec3("endPos", gizmo.lineEndPos - gizmo.pos);
			line_.Draw();
		}
	}
	gizmosQueue_.clear();
	gizmosQueue_.reserve(kGizmoReserveSize);
}

void GizmosRenderer::Destroy()
{
	cube_.Destroy();
	line_.Destroy();
	shaderCube_.Destroy();
}

void GizmosRenderer::DrawCube(
	const Vec3f& pos,
	const Vec3f& size,
	const Color4& color)
{
	Gizmos gizmo;
	gizmo.color = color;
	gizmo.pos = pos;
	gizmo.shape = GizmoShape::CUBE;
	gizmo.cubeSize = size;
	gizmosQueue_.push_back(gizmo);
}

void GizmosRenderer::DrawLine(
	const Vec3f& startPos,
	const Vec3f& endPos,
	const Color4& color)
{
	Gizmos gizmo;
	gizmo.color = color;
	gizmo.pos = startPos;
	gizmo.shape = GizmoShape::LINE;
	gizmo.lineEndPos = endPos;
	gizmosQueue_.push_back(gizmo);
}
}
