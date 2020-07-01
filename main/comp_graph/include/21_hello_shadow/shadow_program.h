#pragma once

#include "comp_graph/sample_program.h"
#include "gl/shader.h"
#include "gl/shape.h"
#include "gl/model.h"
#include "sdl_engine/sdl_camera.h"


namespace neko
{
class HelloShadowProgram : public SampleProgram
{
public:
    void OnEvent(const SDL_Event& event) override;

    void Init() override;

    void Update(seconds dt) override;
	void FixedUpdate() override {}

    void Destroy() override;

    void DrawImGui() override;

    void Render() override;
private:
	enum ShadowFlags : std::uint8_t
	{
		NONE = 0u,
		ENABLE_SHADOW = 1u,
		ENABLE_BIAS = 1u << 1u,
		ENABLE_PETER_PANNING = 1u << 2u,
		ENABLE_OVER_SAMPLING = 1u << 3u,
		ENABLE_PCF = 1u << 4u
	};
    struct DirectionalLight
    {
        Vec3f lightPos = Vec3f::one*10.0f;
        Vec3f lightDir = (-Vec3f::one).Normalized();
    };

    struct Transform
    {
        Vec3f position = Vec3f::zero;
        Vec3f scale = Vec3f::one;
        radian_t angle = radian_t(0.0f);
        Vec3f axis = Vec3f::up;
    };
    void RenderScene(const gl::Shader& shader);
	
    gl::RenderQuad floor_{Vec3f::zero, Vec2f::one*5.0f};
    gl::Texture floorTexture_;

    gl::RenderCuboid cube_{Vec3f::zero, Vec3f::one};
    std::array<Transform, 4> cubeTransforms_ = {
        {
	        {Vec3f(0.0f, 3.0f, 0.0f), Vec3f(0.5f)},
        	{Vec3f(2.0f, 1.5f, 1.0f), Vec3f(0.5f)},
        	{Vec3f(4.0f, 0.25f, 4.0f), Vec3f(0.5f)},
        	{Vec3f(-1.0f, 1.5f, 2.0f), Vec3f(0.25f), degree_t(60.0f), Vec3f(1,0,1).Normalized()}
        }
    };
    assimp::Model model_;

    gl::Shader simpleDepthShader_;
    gl::Shader modelShader_;

    sdl::Camera3D camera_;
    Camera2D depthCamera_;

    unsigned int depthMapFbo_ = 0;
    unsigned int depthMap_ = 0;
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    DirectionalLight light_;

    float shadowBias_ = 0.0005f;
    std::uint8_t flags_ = NONE;
};
}