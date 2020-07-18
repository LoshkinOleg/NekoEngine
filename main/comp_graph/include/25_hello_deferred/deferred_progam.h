#pragma once

#include <comp_graph/sample_program.h>
#include "sdl_engine/sdl_camera.h"
#include "gl/shape.h"
#include "gl/shader.h"
#include "gl/texture.h"
#include "gl/model.h"

namespace neko
{
class HelloDeferredProgram : public SampleProgram
{
public:

    void Init() override;

    void Update(seconds dt) override;
    void FixedUpdate() override {}

    void Destroy() override;

    void DrawImGui() override;

    void Render() override;

    void OnEvent(const SDL_Event& event) override;

private:
    enum DeferredFlags : std::uint8_t
    {
        NONE = 0u,
        RESIZE_SCREEN = 1u,
        FORWARD_RENDERING = 1u << 1u
    };

    struct PointLight
    {
        Vec3f position;
        Vec3f color;
    };

    PointLight lights_[32];


    void CreateFramebuffer();
    void RenderScene(const gl::Shader& shader);

    sdl::Camera3D camera_;
    gl::Shader deferredShader_;
    gl::Shader lightingShader_;
    gl::Shader forwardShader_;

    gl::RenderQuad floor_{Vec3f::zero, Vec2f::one*10.0f};
    gl::RenderQuad screenQuad_{Vec3f::zero, Vec2f::one*2.0f};
    gl::RenderCuboid cube_{Vec3f(0.0f,0.5f,0.0f), Vec3f::one};
    assimp::Model model_;

    gl::Texture container_;
    gl::Texture containerSpecular_;
    TextureId whiteTexture_ = 0;

    unsigned int gBuffer_ = 0;
    unsigned int gPosition_ = 0;
    unsigned int gNormal_ = 0;
    unsigned int gAlbedoSpec_ = 0;
    unsigned int rbo_ = 0;
    std::uint8_t flags_ = NONE;
};

}