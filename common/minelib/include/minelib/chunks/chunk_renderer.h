#pragma once
#include "gl/shader.h"
#include "gl/shape.h"

#include "chunk_manager.h"
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
	ChunkRenderer(MinecraftLikeEngine& engine, Camera& camera);

	void Init() override;
	void Update(seconds dt) override;
	void FixedUpdate() override {}
	void Destroy() override;

	void Render() override;

	void DrawImGui();

	void SetCameraParameters(const Camera& camera) const;

	void SetLightParameters() const;

	void RenderScene(const gl::Shader& shader) const;

private:
	Camera& camera_;
	Camera2D depthCamera_;
	
	MinecraftLikeEngine& engine_;
	ChunkManager& chunkManager_;

	DirectionalLight directionalLight_;

	gl::Shader shader_;
	gl::Shader simpleDepthShader_;

	TextureId atlasTex_ = 0;
	gl::RenderCuboid cube_{Vec3f::zero, Vec3f::one};
	
	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
	unsigned int depthMapFBO = 0;
	unsigned int depthMap_ = 0;

	bool enableShadows = false;
};
}
