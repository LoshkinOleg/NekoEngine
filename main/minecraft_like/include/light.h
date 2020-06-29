#pragma once
#include <mathematics/vector.h>

#include "gl/shader.h"
#include "graphics/color.h"
#include <engine/system.h>
#include "engine/engine.h"
#include <gl/shape.h>

#include <graphics/graphics.h>
namespace neko
{

	class MinecraftLikeEngine;
	struct MoveableCamera3D;
class Light final : public SystemInterface
{
public:
	explicit Light(MinecraftLikeEngine& engine);
	void Init() override;
	void Update(seconds dt) override;
	void Destroy() override;

	void BindShader(Mat4f& model, Mat4f& view, Mat4f& projection, Vec3f pos, gl::RenderCuboid& cube);
	
private:
	gl::Shader lightShader_;
	gl::Shader phongShader_;

	Vec3f lightPos_;

	float time_ = 0.0f;
	const float lightDist_ = 3.5f;
	Color3 lightColor_{ 1,1,1 };
	Color3 objectColor_{ 1.0f,0.23f, 0.1f };
	float ambientStrength_ = 0.1f;
	float diffuseStrength_ = 1.0f;
	float specularStrength_ = 0.5f;
	int specularPow_ = 32;
};
}
