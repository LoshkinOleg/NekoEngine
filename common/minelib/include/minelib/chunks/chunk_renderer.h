#pragma once
#include "gl/shader.h"
#include "gl/shape.h"

#include "graphics/camera.h"
#include "graphics/graphics.h"
#include "graphics/texture.h"
#include "minelib/light.h"

namespace neko
{
class Transform3dManager;
struct MoveableCamera3D;
class MinecraftLikeEngine;
class ChunkRenderManager;

struct ChunkRender
{
	unsigned vbo = 0;
	gl::RenderCuboid cube{Vec3f::zero, Vec3f::one};
};

class ChunkManager;
class ChunkRenderer final : public RenderCommandInterface, public SystemInterface
{
public:
	ChunkRenderer(MinecraftLikeEngine& engine, FpsCamera* camera);

	void Init() override;
	void Update(seconds dt) override;
	void FixedUpdate() override {}
	void Destroy() override;

	void Render() override;

	void DrawImGui();
	void SetCamera(FpsCamera* camera);

private:
	void InitShadow();
	void RenderScene(gl::Shader& shader) const;
	void SetCameraParameters(const Camera& camera, gl::Shader& shader) const;
	void SetShadowParameters(gl::Shader& shader) const;
	void SetLightParameters() const;
	
	FpsCamera* GetCameraPos() const { return camera_; }

private:
	FpsCamera* camera_;

	std::mutex mutex_;

	Camera2D depthCamera_;
	
	MinecraftLikeEngine& engine_;
	ChunkManager& chunkManager_;

	DirectionalLight directionalLight_;

	//Shaders
	gl::Shader shader_;
	gl::Shader simpleDepthShader_;

	TextureId atlasTex_ = 0;
	gl::RenderCuboid cube_{Vec3f::zero, Vec3f::one};

	//Shadow
	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
	unsigned int depthMapFBO = 0;
	unsigned int depthMap_ = 0;
	bool enableShadow = true;

	//Skybox
	TextureId skyboxTexture_ = 0;
	gl::Shader skyboxShader_;
	gl::RenderCubeMap skyboxCube_{ Vec3f::zero };
};
}