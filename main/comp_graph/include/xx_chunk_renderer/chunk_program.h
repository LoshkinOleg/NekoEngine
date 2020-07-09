#pragma once
#include "chunk.h"
#include "comp_graph/sample_program.h"

#include "graphics/camera.h"
#include "minelib/aabb_manager.h"
#include "minelib/blocks/block_manager.h"
#include "minelib/gizmos_renderer.h"

namespace neko
{
const static uint8_t kRenderDist = 8;
const static uint8_t kChunkNumDiam = kRenderDist * 2 + 1;
const static uint16_t kChunkNum = kChunkNumDiam * kChunkNumDiam;

class HelloChunkRenderer final : public SampleProgram
{
public:
	explicit HelloChunkRenderer();
	
    void Init() override;

    void Update(seconds) override;
	void FixedUpdate() override {}

    void Render()override;

    void Destroy() override;

    void DrawImGui() override;

    void OnEvent(const SDL_Event& event) override;
	
	bool RaycastBlockInChunk(Ray& ray,
	                         const Vec3f& origin,
	                         const Vec3f& dir) const;

private:
	MoveableCamera3D camera_;
	BlockManager blockManager_;
	GizmosRenderer gizmosRenderer_;
	
	unsigned uboMatrices_ = 0;
	unsigned vao_ = 0;
	TextureId atlasTex_ = 0;
    gl::Shader shader_;
	std::array<TestChunk, kChunkNum> chunks_;
};
}
