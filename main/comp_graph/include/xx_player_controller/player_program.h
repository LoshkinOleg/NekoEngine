#pragma once

#include <comp_graph/sample_program.h>

#include <gl/shader.h>
#include <gl/shape.h>
#include <graphics/camera.h>
#include <graphics/texture.h>
#include <mathematics/aabb.h>

namespace neko
{
const static float cubeHalfSize = 0.5f;

struct Ray
{
	float hitDist = std::numeric_limits<float>::infinity();
	Vec3f hitPos = Vec3f::zero;
	int hitIndex = -1;
};

enum PlayerState : uint8_t
{
	SPRINTING = 1u << 0u,
	FLYING = 1u << 1u,
};

class PlayerProgram final : public SampleProgram
{
public:
    void Init() override;

    void Update(seconds dt) override;

    void Destroy() override;

    void Render() override;

    void DrawImGui() override;

    void OnEvent(const SDL_Event& event) override;
	
	void CreateCube(const Vec3f& position);
	void PlaceCube(const Vec3f& position);
	
	void DeleteCube(const size_t& index);

	void MovePlayer();
	
	void CheckPlayerPos();
	
	void HeadBobbing();
	
	void CheckBlock();

    static bool IsMoving();

	bool RayCast(
        Ray& out,
        const Vec3f& origin,
		const Vec3f& direction,
        float distance);
private:
	float gravity_ = 36.5f;
    PlayerCamera camera_;
	
	float timer_ = 0.0f;
	
	//Player variables
	Vec3f playerPos_ = Vec3f::back * 3 + Vec3f::up * 2;
	Vec3f playerAabbSize_ = Vec3f(0.5f, 0.9f, 0.5f);
	Aabb3d playerAabb_;
	uint8_t playerState_ = 0;
    float maxReach_ = 6.0f;
	float placeCoolDown_ = 0.2f;
	float placeTimeStamp_ = 0.0f;

	//Movement variables
	Vec3f playerVelocity_ = Vec3f::zero;
	float decelerationSpeed_ = 35.0f;
	float sprintMultiplier_ = 1.3f;
	float speedMultiplier_ = 1.0f;
	
	//Jump Variables
	float playerJumpPower_ = 9.f;
	bool canJump_ = false;

	//Head Bobbing
	Vec3f cameraOffset_ = Vec3f::up * 1.5f;
    float walkingBobbingSpeed_ = 14.0f;
    float bobbingAmount_ = 0.05f;
    float defaultPosY_ = 1.5f;
    float bobbingTimer_ = 0;


	//Cubes
    gl::RenderCuboid cube_{Vec3f::zero, Vec3f::one};
    gl::Shader shader_;
    gl::RenderWireFrameCuboid selectCube_{Vec3f::zero, Vec3f(1.015f)};
    gl::Shader shaderLine_;

    TextureId texture_ = 0;

	std::vector<Vec3f> cubePositions_;
	std::vector<Aabb3d> cubeAabbs_;

	//UI Test
	gl::RenderQuad crossHair_{Vec3f::zero, Vec2f::one};
    gl::Shader uiShader_;
    TextureId crossHairTexture_ = 0;
	Vec2u crossHairSize_ = Vec2u(100, 100);
};
}
