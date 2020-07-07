#pragma once
#include <gl/shader.h>
#include <gl/shape.h>

#include <graphics/graphics.h>
#include <sdl_engine/sdl_engine.h>
#include <graphics/camera.h>
#include <graphics/texture.h>
#include <minelib/light.h>

namespace neko
{
	class Transform3dManager;
	struct MoveableCamera3D;
	class MinecraftLikeEngine;

	class ChunkRenderer final : public RenderCommandInterface, public SystemInterface
	{
	public:
		ChunkRenderer(
			MinecraftLikeEngine& engine,
			MoveableCamera3D& camera,
			EntityViewer& entityViewer);

		void Init() override;

		void Update(seconds dt) override;

		void FixedUpdate() override
		{
		}

		void Destroy() override;

		void Render() override;

		void DrawImGui();

		void SetCameraParameters(Mat4f& model, Mat4f& view, Mat4f& projection, Vec3f pos);

		void SetLightParameters();

	private:

		MoveableCamera3D& camera_;

		gl::RenderCuboid cube_{ Vec3f::zero, Vec3f::one };
		gl::Shader shader_;

		TextureId texture_[3];

		std::mutex updateMutex_;
		MinecraftLikeEngine& engine_;
		EntityViewer& entityViewer_;
		EntityManager& entityManager_;
		ChunkPosManager& chunkPosManager_;
		ChunkStatutManager& chunkStatutManager_;
		ChunkContentManager& chunkContentManager_;
		Transform3dManager& transform3dManager_;

		DirectionalLight directionalLight_;
	};
}