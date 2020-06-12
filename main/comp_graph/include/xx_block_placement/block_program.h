#pragma once

#include <comp_graph/sample_program.h>

#include <gl/shader.h>
#include <gl/shape.h>
#include <graphics/camera.h>
#include <graphics/texture.h>
#include <mathematics/aabb.h>

namespace neko
{
struct Ray
{
	float hitDist = std::numeric_limits<float>::infinity();
	Vec3f hitPos = Vec3f::zero;
	int hitIndex = -1;
};

class BlockProgram final : public SampleProgram
{
public:
    void Init() override;

    void Update(seconds dt) override;

    void Destroy() override;

    void Render() override;

    void DrawImGui() override;

    void OnEvent(const SDL_Event& event) override;

	void CreateCube(const Vec3f& position);
	
	void DeleteCube(const size_t& index);

	bool RayCast(
        Ray& out,
        const Vec3f& origin,
		const Vec3f& direction,
        float distance);
private:
    FpsCamera camera_;
	
    gl::RenderCuboid cube_{Vec3f::zero, Vec3f::one};
    gl::Shader shader_;

    TextureId texture_ = 0;

    const float maxReach_ = 8.0f;
	std::vector<Vec3f> cubePositions_;
	std::vector<Aabb3d> cubeAabbs_;
};
}
