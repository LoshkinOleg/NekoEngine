#pragma once
#include <gl/shader.h>
#include <gl/shape.h>

#include <graphics/graphics.h>
#include <sdl_engine/sdl_engine.h>


namespace neko
{
struct MoveableCamera3D;
class MinecraftLikeEngine;

class ChunkRenderer final : public RenderCommandInterface
{
public:
	ChunkRenderer(MinecraftLikeEngine& engine, MoveableCamera3D& camera, EntityViewer& entityViewer);

	void Render() override;

	void Init();

	void Update(seconds dt);

	void Destroy();

private:
	std::mutex updateMutex_;
	MoveableCamera3D& camera_;
	MinecraftLikeEngine& engine_;
	EntityViewer& entityViewer_;

	seconds timeSinceInit_ = seconds(0.0f);
	
	//Initialization data for test
	gl::RenderCuboid cube_{Vec3f::zero, Vec3f::one / 2.0f};
	gl::Shader shader_;
	const EntityMask kCubeComponentType_ = EntityMask(ComponentType::OTHER_TYPE);
};
}
