#include "xx_chunk_renderer/chunk_program.h"


#include "minelib/aabb_manager.h"
#include "stb_image.h"

namespace neko
{
HelloChunkRenderer::HelloChunkRenderer() : gizmosRenderer_(camera_)
{
	
}

void HelloChunkRenderer::Init()
{
	stbi_set_flip_vertically_on_load(true);
	//camera_.position = Vec3f(kChunkSize * kRenderDist + kChunkSize / 2.0f, 1.0f, kChunkSize * kRenderDist + kChunkSize / 2.0f);
	camera_.position = Vec3f::up;
	camera_.LookAt(Vec3f::one);
		
	blockManager_.Init();
	gizmosRenderer_.Init();
	for (auto& chunk : chunks_)
	{
		chunk.Init();
		for (uint16_t id = 0; id < kChunkBlockCount; ++id)
		{
			chunk.SetBlock(blockManager_.GetBlock(rand() % 6 + 1), id);
		}
	
		for (uint8_t x = 0; x < kChunkNumDiam; ++x)
		{
			for (uint8_t z = 0; z < kChunkNumDiam; ++z)
			{
				chunks_[x + z * kChunkNumDiam].SetChunkPos(Vec3f(x, -1, z));
			}
		}
		chunk.SetVbo();
	}
	
	auto& config = BasicEngine::GetInstance()->config;
	shader_.LoadFromFile(
		config.dataRootPath + "shaders/minecraft_like/base/cube_vertex.vert",
		config.dataRootPath + "shaders/minecraft_like/base/cube.frag");
	atlasTex_ = stbCreateTexture(config.dataRootPath + "sprites/atlas.png", gl::Texture::CLAMP_WRAP);
	
	//Set Uniform Buffer
	{
		const unsigned uniformReflection = glGetUniformBlockIndex(shader_.GetProgram(), "Matrices");
		glUniformBlockBinding(shader_.GetProgram(), uniformReflection, 0);
		
		glGenBuffers(1, &uboMatrices_);
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices_);
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(Mat4f), nullptr, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices_, 0, 2 * sizeof(Mat4f));
	}
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void HelloChunkRenderer::Update(seconds dt)
{
	camera_.Update(dt);
}

void HelloChunkRenderer::Render()
{
	const Mat4f camProj = camera_.GenerateProjectionMatrix();
	const Mat4f camView = camera_.GenerateViewMatrix();
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices_);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Mat4f), &camProj);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Mat4f), sizeof(Mat4f), &camView);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	
	shader_.Bind();
	glBindTexture(GL_TEXTURE_2D, atlasTex_);
	for (auto& chunk : chunks_)
	{
		glBindVertexArray(chunk.GetVao());
		shader_.SetVec3("chunkPos", chunk.GetChunkPos());
		chunk.Render();
	}
	
	Ray rayOut;
	const Vec3f direction = camera_.reverseDirection.Normalized() * -1;
	if (RaycastBlockInChunk(rayOut, camera_.position, direction))
	{
		glLineWidth(3.0f);
		gizmosRenderer_.DrawCube(rayOut.hitPos, Vec3f(kCubeHalfSize + 0.01f) * 2, Color::black);
		gizmosRenderer_.Render();
	}
}

void HelloChunkRenderer::Destroy()
{
	for (auto& chunk : chunks_)
	{
		chunk.Destroy();
	}
	blockManager_.Destroy();
	gizmosRenderer_.Destroy();
}

void HelloChunkRenderer::DrawImGui()
{
}

void HelloChunkRenderer::OnEvent(const SDL_Event& event)
{
	camera_.OnEvent(event);
}

bool HelloChunkRenderer::RaycastBlockInChunk(Ray& ray, const Vec3f& origin, const Vec3f& dir) const
{
	const Vec3f chunkPos = chunks_[0].GetChunkPos();
	
	float rayDist;
	for (auto& blockId : chunks_[0].GetBlockIds())
	{
		const Vec3f blockPos = Vec3f(BlockIdToPos(blockId)) + chunkPos * kChunkSize;
		
		Aabb3d aabb;
		aabb.SetFromCenter(blockPos, Vec3f(kCubeHalfSize));
		if (aabb.IntersectRay(dir, origin, rayDist) && rayDist > 0 && rayDist < ray.hitDist)
		{
			ray.hitId = blockId;
			ray.hitDist = rayDist;
			ray.hitPos = blockPos;
			ray.hitAabb = aabb;
		}
	}
	return ray.hitId != INVALID_INDEX;
}
}
