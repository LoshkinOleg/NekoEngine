#pragma once

#include "comp_graph/sample_program.h"
#include "gl/shape.h"
#include "gl/shader.h"
#include "gl/texture.h"
#include "mathematics/vector.h"

namespace neko
{
class HelloCoordsProgram : public SampleProgram
{
public:
    void Init() override;

    void Update(seconds) override;
	void FixedUpdate() override {}

    void Render()override;

    void Destroy() override;

    void DrawImGui() override;

    void OnEvent(const SDL_Event& event) override;

private:
    gl::RenderCuboid cube_{Vec3f::zero, Vec3f::one};
    const static size_t cubeNumbers_ = 10;
    Vec3f cubePositions[cubeNumbers_] =
    {
        Vec3f(0.0f, 0.0f, 0.0f),
        Vec3f(2.0f, 5.0f, -15.0f),
        Vec3f(-1.5f, -2.2f, -2.5f),
        Vec3f(-3.8f, -2.0f, -12.3f),
        Vec3f(2.4f, -0.4f, -3.5f),
        Vec3f(-1.7f, 3.0f, -7.5f),
        Vec3f(1.3f, -2.0f, -2.5f),
        Vec3f(1.5f, 2.0f, -2.5f),
        Vec3f(1.5f, 0.2f, -1.5f),
        Vec3f(-1.3f, 1.0f, -1.5f)
    };
    gl::Shader shader_;
    TextureId  textureWall_;

    Mat4f view{Mat4f::Identity};
    Mat4f projection{Mat4f::Identity};

    seconds timeSinceInit_{0};
};
}