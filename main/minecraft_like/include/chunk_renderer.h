#pragma once
#include <gl/shader.h>
#include <gl/shape.h>

#include <graphics/graphics.h>
#include <sdl_engine/sdl_engine.h>
#include <graphics/camera.h>
#include <graphics/texture.h>
#include <light.h>

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

	void DrawImGui();

	void GenerateDepthBuffer();
	
	void SetCameraParameters(Mat4f& model, Mat4f& view, Mat4f& projection, Vec3f pos, TextureId texture);
	
	void SetLightParameters();

private:
	
	MoveableCamera3D& camera_;

	gl::RenderCuboid cube_{ Vec3f::zero, Vec3f::one };
	gl::Shader shader_;
	gl::Shader simpleDepthShader_;

	TextureId texture_[3];

	std::mutex updateMutex_;
	MinecraftLikeEngine& engine_;
	EntityViewer& entityViewer_;

	DirectionalLight directionalLight_;
	Camera2D depthCamera_;
	unsigned int depthMapFbo_ = 0;
	unsigned int depthMap_ = 0;
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	float shadowBias_ = 0.0005f;
};
}