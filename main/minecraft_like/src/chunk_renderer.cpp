#include "chunk_renderer.h"

#include <graphics/camera.h>
#ifdef EASY_PROFILE_USE
#include <easy/profiler.h>
#endif

#include "gl/texture.h"
#include "minecraft_like_engine.h"

namespace neko
{
ChunkRenderer::ChunkRenderer(
	MinecraftLikeEngine& engine,
	MoveableCamera3D& camera,
	EntityViewer& entityViewer)
	: camera_(camera),
	engine_(engine),
	entityViewer_(entityViewer)
{
}

void ChunkRenderer::Init()
{
	const auto& config = BasicEngine::GetInstance()->config;
	shader_.LoadFromFile(
		config.dataRootPath + "shaders/minecraft_like/light.vert",
		config.dataRootPath + "shaders/minecraft_like/light.frag");
	simpleDepthShader_.LoadFromFile(config.dataRootPath + "shaders/21_hello_shadow/simple_depth.vert",
		config.dataRootPath + "shaders/21_hello_shadow/simple_depth.frag");
	
	texture_[0] = gl::stbCreateTexture(config.dataRootPath + "sprites/blocks/dirt.jpg");
	texture_[1] = gl::stbCreateTexture(config.dataRootPath + "sprites/blocks/stone.jpg");
	texture_[2] = gl::stbCreateTexture(config.dataRootPath + "sprites/blocks/diamond_ore.jpg");
	cube_.Init();
	
	//Depth Buffer
	GenerateDepthBuffer();
}

void ChunkRenderer::DrawImGui(){

	ImGui::Begin("Light");
		Vec3f lightDirection = directionalLight_.direction_;
		if(ImGui::InputFloat3("Direction", &lightDirection[0])) {
			directionalLight_.direction_ = lightDirection;
		}

		Vec3f lightPosition = directionalLight_.position_;
		if (ImGui::InputFloat3("Position", &lightPosition[0])) {
			directionalLight_.position_ = lightPosition;
		}
	
	ImGui::End();
}


void ChunkRenderer::Update(seconds dt)
{
	std::lock_guard<std::mutex> lock(updateMutex_);
	GizmosLocator::get().DrawCube(directionalLight_.position_, Vec3f(.5f), Color4(1, 1, 1, 1));
}

void ChunkRenderer::Render()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("ChunkRenderer::Render");
#endif
	if (shader_.GetProgram() == 0) return;

	std::lock_guard<std::mutex> lock(updateMutex_);
	
	Mat4f view = camera_.GenerateViewMatrix();
	Mat4f projection = camera_.GenerateProjectionMatrix();

	SetLightParameters();

	for (size_t i = 0; i < INIT_ENTITY_NMB; i++)
	{
		if (!engine_.entityManager_.HasComponent(i, static_cast<EntityMask>(ComponentType::CHUNK))) { continue; }
		Chunk chunk = engine_.componentsManagerSystem_.chunkManager_.GetComponent(i);

#ifdef EASY_PROFILE_USE
		EASY_BLOCK("ChunkRenderer::Render::Chunk");
#endif

		for (int x = 0; x < kChunkSize; x++)
		{
			for (int y = 0; y < kChunkSize; y++)
			{
				for (int z = 0; z < kChunkSize; z++)
				{
#ifdef EASY_PROFILE_USE
					EASY_BLOCK("ChunkRenderer::Render::Air");
#endif
					int blockID = chunk.GetBlockId(Vec3i(x, y, z));
					if (blockID != 0)
					{
						if (texture_[blockID - 1] == INVALID_TEXTURE_ID) continue;
#ifdef EASY_PROFILE_USE
						EASY_BLOCK("ChunkRenderer::Render::Cube");
#endif
						Mat4f model = Mat4f::Identity;	
						model = Transform3d::Translate(model, Vec3f(x, y, z) + chunk.GetChunkPos());

						SetCameraParameters(model, view, projection, camera_.position, texture_[blockID - 1]);

						glBindTexture(GL_TEXTURE_2D, texture_[blockID - 1]); //bind texture id to texture slot
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
						
						cube_.Draw();
					}
				}
			}
		}
		Mat4f model = Mat4f::Identity;
		model = Transform3d::Translate(model, Vec3f(0, 2, 0));

		SetCameraParameters(model, view, projection, camera_.position, texture_[1]);

		glBindTexture(GL_TEXTURE_2D, texture_[1]); //bind texture id to texture slot
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		cube_.Draw();
	}
}

void ChunkRenderer::GenerateDepthBuffer() {
	glEnable(GL_DEPTH_TEST);
	glGenFramebuffers(1, &depthMapFbo_);
	glGenTextures(1, &depthMap_);
	glBindTexture(GL_TEXTURE_2D, depthMap_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFbo_);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap_, 0);
	glDrawBuffers(0, GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LogDebug("[Error] Shadow depth map framebuffer is incomplete");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	depthCamera_.SetSize(Vec2f::one * 4.0f);
	depthCamera_.position = directionalLight_.position_;
	depthCamera_.reverseDirection = -directionalLight_.direction_;
}
	
void ChunkRenderer::SetLightParameters() {
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	const auto lightView = depthCamera_.GenerateViewMatrix();
	const auto lightProjection = depthCamera_.GenerateProjectionMatrix();
	const auto lightSpaceMatrix = lightProjection * lightView;

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFbo_);
	glClear(GL_DEPTH_BUFFER_BIT);
	simpleDepthShader_.Bind();
	simpleDepthShader_.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

	glCullFace(GL_FRONT);
	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	shader_.Bind();
	shader_.SetMat4("view", camera_.GenerateViewMatrix());
	shader_.SetMat4("projection", camera_.GenerateProjectionMatrix());
	shader_.SetMat4("lightSpaceMatrix", lightSpaceMatrix);
	shader_.SetTexture("shadowMap", depthMap_, 3);
	shader_.SetBool("enableBias", 1);
	shader_.SetBool("enableShadow", 1);
	shader_.SetBool("enableOverSampling", 1);
	shader_.SetBool("enablePcf", 1);
	shader_.SetVec3("viewPos", camera_.position);
	shader_.SetFloat("bias", shadowBias_);
	shader_.SetVec3("light.lightDir", directionalLight_.direction_);
}


void ChunkRenderer::SetCameraParameters(Mat4f& model, Mat4f& view, Mat4f& projection, Vec3f pos, TextureId texture) {

	const auto inverseTransposeModel = model.Inverse().Transpose();
	shader_.SetMat4("inverseTransposeModel", inverseTransposeModel);

	shader_.SetMat4("transposeInverseModel", model.Inverse().Transpose());

	//Depth
	simpleDepthShader_.SetMat4("model", model);
	simpleDepthShader_.SetMat4("transposeInverseModel", model.Inverse().Transpose());

	//Normal
	shader_.SetMat4("transposeInverseModel", model.Inverse().Transpose());
	shader_.SetMat4("model", model);
	shader_.SetMat4("view", view);
	shader_.SetMat4("projection", projection);
	shader_.SetVec3("viewPos", pos);
	shader_.SetTexture("material.texture_diffuse1", texture, 0);

}

	
void ChunkRenderer::Destroy()
{
	cube_.Destroy();
	shader_.Destroy();
	gl::DestroyTexture(texture_[0]);
	gl::DestroyTexture(texture_[1]);
	gl::DestroyTexture(texture_[2]);
	glDeleteFramebuffers(1, &depthMapFbo_);
	glDeleteTextures(1, &depthMap_);
}

}