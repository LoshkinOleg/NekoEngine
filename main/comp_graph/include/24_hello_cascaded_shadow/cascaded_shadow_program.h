#pragma once
#include "comp_graph/sample_program.h"
#include "gl/texture.h"
#include "gl/shape.h"
#include "gl/shader.h"
#include "gl/model.h"
#include "sdl_engine/sdl_camera.h"

namespace neko
{
class HelloCascadedShadowProgram : public SampleProgram
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

    enum CascadedShadowFlags : std::uint8_t
    {
        NONE = 0u,
        ENABLE_CASCADE_COLOR = 1u,
        ENABLE_AABB_CASCADE = 1u<<1u
    };

	struct DirectionalLight
	{
	    Vec3f position = Vec3f::zero;
		Vec3f direction = -Vec3f::one;
		Mat4f lightSpaceMatrix;
	};

	[[nodiscard]] Camera2D CalculateOrthoLight(float cascadeNear, float cascadeFar, Vec3f lightDir) const;
	void ShadowPass(int cascadeIndex);
	void RenderScene(const gl::Shader& shader);

	sdl::Camera3D camera_;

	gl::Shader simpleDepthShader_;
	gl::Shader shadowShader_;

	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned fbo_ = 0;
	std::array<unsigned, 3> shadowMaps_{};
	float cascadedNearRatio_ = 0.1f;
	float cascadedMiddleRatio_ = 0.6f;
	float shadowBias_ = 0.005f;
	std::uint8_t flags_ = NONE;
	std::array<DirectionalLight, 3> lights_;

	gl::RenderQuad plane_{Vec3f::zero, Vec2f::one};
	assimp::Model dragonModel_;
	gl::Texture brickWall_;
	TextureId whiteTexture_ = 0;
};
}
