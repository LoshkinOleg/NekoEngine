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
		const Color4& color = Color::red) = 0;

	/**
	 * \brief Generate a line.
	 */
	virtual void DrawLine(
		const Vec3f& startPos,
		const Vec3f& endPos,
		const Color4& color = Color::red) = 0;
	
	virtual Vec3f GetCameraPos() const = 0;
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
		[[maybe_unused]] const Color4& color = Color::red) override
	{
	}

	void DrawLine(
		[[maybe_unused]] const Vec3f& startPos,
		[[maybe_unused]] const Vec3f& endPos,
		[[maybe_unused]] const Color4& color = Color::red) override
	{
	}

	Vec3f GetCameraPos() const override { return {}; }
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
	explicit GizmosRenderer(Camera& camera);

	void Init() override;

	void Update(seconds dt) override;
	void FixedUpdate() override {}

	void Render() override;

	void Destroy() override;

	void Start();
	void Stop();
	

	void DrawCube(
		const Vec3f& pos,
		const Vec3f& size = Vec3f::one,
		const Color4& color = Color::red) override;

	void DrawLine(
		const Vec3f& startPos,
		const Vec3f& endPos,
		const Color4& color = Color::red) override;

	Vec3f GetCameraPos() const override { return camera_.position; }
	
private:
	std::mutex updateMutex_;

	Camera& camera_;

	gl::RenderWireFrameCuboid cube_{Vec3f::zero, Vec3f::one};
	gl::RenderLine3d line_{Vec3f::zero, Vec3f::one};

	gl::Shader shaderCube_;
	gl::Shader shaderLine_;

	std::vector<Gizmos> gizmosQueue_;
	bool isRunning_ = false;
};

using GizmosLocator = Locator<IGizmosRenderer, NullGizmosRenderer>;
}
