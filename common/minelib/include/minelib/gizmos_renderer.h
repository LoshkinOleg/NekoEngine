#pragma once
#include <gl/shader.h>
#include <gl/shape.h>

#include <graphics/camera.h>
#include <graphics/color.h>
#include <graphics/graphics.h>
#include <utilities/service_locator.h>

namespace neko
{
struct MoveableCamera3D;
class MinecraftLikeEngine;

const static size_t kGizmoReserveSize = 128;

enum class GizmoShape : std::uint8_t
{
	CUBE = 0u,
	LINE,
};

struct Gizmos
{
	Gizmos() {}
	
	Vec3f pos = Vec3f::zero;
	Color4 color = Color::red;
	GizmoShape shape = GizmoShape::CUBE;
	float lineThickness = 1.0f;

	union
	{
		Vec3f cubeSize = Vec3f::zero;
		Vec3f lineEndPos;
	};
};

//-----------------------------------------------------------------------------
// GizmosManagerInterface
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
class IGizmosRenderer
{
public:
	/**
	 * \brief Generate a wire cube.
	 */
	virtual void DrawCube(
		const Vec3f& pos,
		const Vec3f& size = Vec3f::one,
		const Color4& color = Color::red,
		float lineThickness = 1.0f) = 0;

	/**
	 * \brief Generate a line.
	 */
	virtual void DrawLine(
		const Vec3f& startPos,
		const Vec3f& endPos,
		const Color4& color = Color::red,
		float lineThickness = 1.0f) = 0;
	
	virtual void SetCamera(Camera* camera) = 0;
	virtual Camera* GetCamera() const = 0;
};

//-----------------------------------------------------------------------------
// NullGizmoManager
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
class NullGizmosRenderer final : public IGizmosRenderer
{
	void DrawCube(
		[[maybe_unused]] const Vec3f& pos,
		[[maybe_unused]] const Vec3f& size = Vec3f::one,
		[[maybe_unused]] const Color4& color = Color::red,
		[[maybe_unused]] float lineThickness = 1.0f) override
	{
	}

	void DrawLine(
		[[maybe_unused]] const Vec3f& startPos,
		[[maybe_unused]] const Vec3f& endPos,
		[[maybe_unused]] const Color4& color = Color::red,
		[[maybe_unused]] float lineThickness = 1.0f) override
	{
	}
	
	void SetCamera([[maybe_unused]] Camera* camera) override {}
	Camera* GetCamera() const override { return nullptr; }
};

//-----------------------------------------------------------------------------
// GizmosRenderer
//-----------------------------------------------------------------------------
/// \brief Draw gizmos
class GizmosRenderer final : public RenderCommandInterface,
                             public SystemInterface,
                             public IGizmosRenderer
{
public:
	explicit GizmosRenderer(Camera* camera);

	void Init() override;

	void Update(seconds dt) override;
	void FixedUpdate() override {}

	void Render() override;

	void Destroy() override;

	void DrawCube(
		const Vec3f& pos,
		const Vec3f& size = Vec3f::one,
		const Color4& color = Color::red,
		float lineThickness = 1.0f) override;

	void DrawLine(
		const Vec3f& startPos,
		const Vec3f& endPos,
		const Color4& color = Color::red,
		float lineThickness = 1.0f) override;

	void SetCamera(Camera* camera) override;
	Camera* GetCamera() const override { return camera_; }
	
private:
	Camera* camera_;

	gl::RenderWireFrameCuboid cube_{Vec3f::zero, Vec3f::one};
	gl::RenderLine3d line_{Vec3f::zero, Vec3f::one};

	gl::Shader shaderCube_;
	gl::Shader shaderLine_;

	std::vector<Gizmos> gizmosQueue_;
};

using GizmosLocator = Locator<IGizmosRenderer, NullGizmosRenderer>;
}
