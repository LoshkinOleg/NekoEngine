#pragma once
#include <gl/shader.h>
#include <gl/shape.h>

#include <graphics/graphics.h>
#include <sdl_engine/sdl_engine.h>
#include <graphics/camera.h>
#include <graphics/texture.h>


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

	void Destroy() override;

	void Render() override;

private:

	MoveableCamera3D& camera_;

	gl::RenderCuboid cube_{ Vec3f::zero, Vec3f::one };
	gl::Shader shader_;

	TextureId texture_[3];

	std::mutex updateMutex_;
	MinecraftLikeEngine& engine_;
	EntityViewer& entityViewer_;

	//Light
	/*gl::Shader lightShader_;
	gl::Shader phongShader_;

	Vec3f lightPos_{10.0f,50.0f,10.0f};

	float time_ = 0.0f;
	const float lightDist_ = 3.5f;
	Color3 lightColor_{ 1,1,1 };
	Color3 objectColor_{ 1.0f,0.23f, 0.1f };
	float ambientStrength_ = 0.1f;
	float diffuseStrength_ = 1.0f;
	float specularStrength_ = 0.5f;
	int specularPow_ = 32;*/
};
}