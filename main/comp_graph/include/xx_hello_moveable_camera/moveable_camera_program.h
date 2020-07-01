#pragma once

#include <comp_graph/sample_program.h>
#include <gl/shader.h>
#include <gl/shape.h>
#include <graphics/camera.h>

namespace neko
{
class MoveableCameraProgram final : public SampleProgram
{
public:
    void Init() override;

    void Update(seconds dt) override;
    void FixedUpdate() override;

    void Destroy() override;

    void Render() override;

    void DrawImGui() override;

    void OnEvent(const SDL_Event& event) override;

private:
    FpsCamera camera_;
	
    gl::RenderCuboid cube_{Vec3f::zero, Vec3f::one};
    gl::Shader shader_;

    TextureId texture_ = 0;
};
}