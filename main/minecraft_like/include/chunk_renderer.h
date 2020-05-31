#pragma once
#include <gl/shader.h>
#include <gl/shape.h>

#include <graphics/graphics.h>
#include <sdl_engine/sdl_engine.h>

#include "../../inputs/include/input/sample_program.h"
#include "graphics/camera.h"
#include "graphics/texture.h"


namespace neko
{
struct MoveableCamera3D;
class MinecraftLikeEngine;

class ChunkRenderer final : public RenderCommandInterface, public SampleProgram
{
public:
	ChunkRenderer(MinecraftLikeEngine& engine, MoveableCamera3D& camera, EntityViewer& entityViewer);

	void Init() override;

	void Update(seconds dt) override;

	void Destroy() override;

	void Render() override;

	void DrawImGui() override;

	void OnEvent(const SDL_Event& event) override;

private:

	MoveableCamera3D& camera_;

	gl::RenderCuboid cube_{ Vec3f::zero, Vec3f::one };
	gl::Shader shader_;

	TextureId texture_;

	std::mutex updateMutex_;
	MinecraftLikeEngine& engine_;
	EntityViewer& entityViewer_;
	

	Vec3f cubePositions[9] =
	{
		Vec3f(0.0f, 0.0f, 0.0f),
		Vec3f(1.0f, 1.0f, 0.0f),
		Vec3f(2.0f, 2.0f, 0.0f),
		Vec3f(3.0f, 3.0f, 0.0f),
		Vec3f(4.0f, 4.0f, 0.0f),
		Vec3f(0.0f, -1.0f, 1.0f),
		Vec3f(0.0f, -2.0f, 2.0f),
		Vec3f(0.0f, -3.0f, 3.0f),
		Vec3f(0.0f, -4.0f, 4.0f),
	};
};
}
