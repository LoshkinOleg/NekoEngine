#pragma once
#include "comp_graph/sample_program.h"

#include "graphics/camera.h"

#include "minelib/aabb_manager.h"
#include "minelib/blocks/block_manager.h"
#include "minelib/chunks/chunk_manager.h"
#include "minelib/gizmos_renderer.h"
#include "minelib/minecraft_like_engine.h"
#include "minelib/player/player_controller.h"

namespace neko
{
const static uint16_t kRenderDist = 4;
const static uint16_t kChunkNumDiam = kRenderDist * 2 + 1;
const static uint16_t kChunkNum = kChunkNumDiam * kChunkNumDiam * kChunkNumDiam;

class HelloChunkRenderer final : public SampleProgram
{
public:
	explicit HelloChunkRenderer();
	
    void Init() override;

    void Update(seconds) override;
	void FixedUpdate() override;

    void Render()override;

    void Destroy() override;

    void DrawImGui() override;

    void OnEvent(const SDL_Event& event) override;

private:
	MinecraftLikeEngine engine_;
	GizmosRenderer gizmosRenderer_;
	PlayerController playerController_;
	EntityManager& entityManager_;
	
	BlockManager& blockManager_;
	UiManager& uiManager_;
	
	AabbManager& aabbManager_;

	ChunkManager& chunkManager_;
	PlayerManager& playerManager_;

	ChunkRenderer chunkRenderer_;
	
	std::array<Entity, kChunkNum> chunks_{};
	
	TextureId skyboxTexture_ = 0;
	gl::Shader skyboxShader_;
	gl::RenderCubeMap skyboxCube_{Vec3f::zero};
};
}
