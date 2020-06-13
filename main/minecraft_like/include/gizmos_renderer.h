#pragma once
#include <gl/shader.h>
#include <gl/shape.h>

#include <graphics/graphics.h>
#include <sdl_engine/sdl_engine.h>
#include <graphics/camera.h>
#include <graphics/line.h>

namespace neko
{
struct MoveableCamera3D;
class MinecraftLikeEngine;

enum class GizmoShape : std::uint8_t
{
	NONE = 0u,
	CUBE = 1u,
	LINE = 1u << 1u,
};

struct Gizmos
{
	Gizmos()
	{
	}

	GizmoShape shape = GizmoShape::NONE;
	Color4 color = Color4(1.0f, 0.0f, 0.0f, 1.0f);
	Vec3f pos = Vec3f(0.0f, 0.0f, 0.0f);

	union
	{
		Vec3f cubeSize = Vec3f(0.0f, 0.0f, 0.0f);
		Vec3f lineEndPos;
	};
};

//-----------------------------------------------------------------------------
// LogManagerInterface
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
class GizmosManagerInterface
{
protected:
	~GizmosManagerInterface() = default;

public:
	/**
	 * \brief Generate a wire cube.
	 */
	virtual void DrawCube(
		Vec3f pos = Vec3f::zero,
		Vec3f size = Vec3f::one,
		Color4 color = Color4(1.0f, 0.0f, 0.0f, 1.0f)) = 0;

	/**
	 * \brief Generate a wire cube.
	 */
	virtual void DrawLine(
		Vec3f startPos = Vec3f::zero,
		Vec3f endPos = Vec3f::one,
		Color4 color = Color4(1.0f, 0.0f, 0.0f, 1.0f)) = 0;

protected:
	/**
	 * \brief Retrieves the gizmo history
	 */
	std::vector<Gizmos> gizmosQueue_;
};

//-----------------------------------------------------------------------------
// NullLogManager
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
class NullGizmoManager final : public GizmosManagerInterface
{
	void DrawCube(Vec3f pos, Vec3f size, Color4 color) override
	{
		return;
	}

	void DrawLine(Vec3f startPos, Vec3f endPos, Color4 color) override
	{
		return;
	}
};

//-----------------------------------------------------------------------------
// LogManager
//-----------------------------------------------------------------------------
/// \brief Draw gizmos
class GizmosRenderer final : public RenderCommandInterface,
                             public SystemInterface,
                             public GizmosManagerInterface
{
public:
	GizmosRenderer(MinecraftLikeEngine& engine, MoveableCamera3D& camera);

	void Init() override;

	void Update(seconds dt) override;

	void Render() override;
	
	void Destroy() override;

	void DrawCube(Vec3f pos, Vec3f size, Color4 color) override;

	void DrawLine(Vec3f startPos, Vec3f endPos, Color4 color) override;

private:

	std::mutex updateMutex_;
	MinecraftLikeEngine& engine_;

	MoveableCamera3D& camera_;

	gl::RenderCuboid cube_{Vec3f::zero, Vec3f::one};
	gl::RenderCuboid line_{Vec3f::zero, Vec3f::one};

	gl::Shader shaderCube_;

	std::vector<Gizmos> gizmosQueue_;
};

using GizmosLocator = Locator<GizmosManagerInterface, NullGizmoManager>;
}
