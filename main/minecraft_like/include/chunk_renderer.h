#pragma once
#include <gl/shader.h>
#include <gl/shape.h>

#include <graphics/graphics.h>
#include <sdl_engine/sdl_engine.h>
#include <graphics/camera.h>
#include <graphics/texture.h>
#include "frustum.h"


namespace neko
{
struct MoveableCamera3D;
class MinecraftLikeEngine;

class ChunkRenderer final : public RenderCommandInterface, public SystemInterface
{
public:
	ChunkRenderer(MinecraftLikeEngine& engine, MoveableCamera3D& camera, EntityViewer& entityViewer);

	void Init() override;

	void Update(seconds dt) override;
	void FixedUpdate() override {}

	void Destroy() override;

	void Render() override;

private:

	MoveableCamera3D& camera_;
	Frustum frustum_;

	gl::RenderCuboid cube_{ Vec3f::zero, Vec3f::one };
	gl::Shader shader_;

	TextureId texture_[3];

	std::mutex updateMutex_;
	MinecraftLikeEngine& engine_;
	EntityViewer& entityViewer_;
};
}