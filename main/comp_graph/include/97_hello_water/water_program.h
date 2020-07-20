#pragma once
#include "comp_graph/sample_program.h"
#include "gl/texture.h"
#include "gl/model.h"
#include "gl/shader.h"
#include "gl/shape.h"
#include "sdl_engine/sdl_camera.h"

namespace neko
{
class HelloWaterProgram : public SampleProgram
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
	void CreateFramebuffer();
	void CreateDepthbuffer();
	
	assimp::Model model_;
	gl::RenderQuad quad_{Vec3f::zero, Vec2f::one};
	gl::RenderCuboid skyboxCube_{Vec3f::zero, Vec3f::one*2.0f};
	TextureId skyboxTexture_ = 0;
	sdl::Camera3D camera_;
	gl::Shader modelShader_;
	gl::Shader waterShader_;
	gl::Shader skyboxShader_;
	gl::Texture dudvTexturer_;
	gl::Texture normalMap_;

	const Vec2u reflectionFrameSize = Vec2u(480, 240);
	unsigned int reflectionFramebuffer_;
	unsigned int reflectionColorBuffer_;
	unsigned int reflectionDepthBuffer_;
	const Vec2u refractionFrameSize = Vec2u(480, 240);
	unsigned int refractionFramebuffer_;
	unsigned int refractionColorBuffer_;
	unsigned int refractionDepthBuffer_;

	unsigned int depthFramebuffer_;
	unsigned int depthBuffer_;
	float dt_ = 0.0f;
	float waterHeight_ = 1.0f;
	bool resizeScreen_ = false;
};
}
