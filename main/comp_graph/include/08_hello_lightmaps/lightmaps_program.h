#pragma once
#include "comp_graph/sample_program.h"
#include "graphics/texture.h"
#include "gl/shader.h"
#include "gl/shape.h"
#include "sdl_engine/sdl_camera.h"

namespace neko
{
class HelloLightMapsProgram : public SampleProgram
{
public:
	void Init() override;
	void Update(seconds dt) override;
	void Destroy() override;
	void DrawImGui() override;
	void Render() override;
	void OnEvent(const SDL_Event& event) override;

private:
	gl::Shader containerShader_;
	gl::Shader lampShader_;
	TextureId containerDiffuse_;
	TextureId containerSpecular_;
	int specularPow_ = 32;

	gl::RenderCuboid cube_{ Vec3f::zero, Vec3f::one };

	sdl::Camera3D camera_;
	float time_ = 0.0f;
	Vec3f lightPos_;
	const float lightDist_ = 3.0f;
	float ambientStrength_ = 0.1f;
	float diffuseStrength_ = 1.0f;
	float specularStrength_ = 0.5f;
};
}
