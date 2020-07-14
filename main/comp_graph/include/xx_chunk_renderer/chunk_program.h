#pragma once
#include "comp_graph/sample_program.h"

#include "graphics/camera.h"
#include "minelib/aabb_manager.h"
#include "minelib/blocks/block_manager.h"
#include "minelib/chunks/chunk_manager.h"
#include "minelib/gizmos_renderer.h"
#include "minelib/minecraft_like_engine.h"

namespace neko
{
const static uint16_t kRenderDist = 6;
const static uint16_t kChunkNumDiam = kRenderDist * 2 + 1;
const static uint16_t kChunkNum = kChunkNumDiam * kChunkNumDiam * kChunkNumDiam;

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

private:
	MoveableCamera3D camera_;
	
	MinecraftLikeEngine engine_;
	GizmosRenderer gizmosRenderer_;
	EntityManager& entityManager_;
	BlockManager& blockManager_;
	AabbManager& aabbManager_;
	//Transform3dManager& transform3dManager_;

	ChunkManager& chunkManager_;

	ChunkRenderer chunkRenderer_;

	const uint8_t maxReach_ = 8u;
	std::array<Entity, kChunkNum> chunks_{};
	
	TextureId skyboxTexture_ = 0;
	gl::Shader skyboxShader_;
	gl::RenderCubeMap skyboxCube_{Vec3f::zero};
};
}
