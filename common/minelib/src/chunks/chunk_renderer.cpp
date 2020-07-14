#include "minelib/chunks/chunk_renderer.h"

#ifdef EASY_PROFILE_USE
#include <easy/profiler.h>
#endif

#include <stb_image.h>

#include "gl/texture.h"
#include "graphics/camera.h"
#include "minelib/minecraft_like_engine.h"

namespace neko
{
ChunkRenderer::ChunkRenderer(MinecraftLikeEngine& engine, Camera& camera)
	: camera_(camera),
	  engine_(engine),
	  chunkManager_(engine.componentsManagerSystem.chunkManager)
{
}

void ChunkRenderer::Init()
{
	stbi_set_flip_vertically_on_load(true);
	const auto& config = BasicEngine::GetInstance()->config;
	/*shader_.LoadFromFile(
		config.dataRootPath + "shaders/minecraft_like/light.vert",
		config.dataRootPath + "shaders/minecraft_like/light.frag");*/
	shader_.LoadFromFile(
		config.dataRootPath + "shaders/minecraft_like/cubeShadow.vert",
		config.dataRootPath + "shaders/minecraft_like/cubeShadow.frag");
	simpleDepthShader_.LoadFromFile(
		config.dataRootPath + "shaders/minecraft_like/shadowMappingDepth.vert",
		config.dataRootPath + "shaders/minecraft_like/shadowMappingDepth.frag");
	atlasTex_ = stbCreateTexture(config.dataRootPath + "sprites/atlas.png", gl::Texture::CLAMP_WRAP);
	glEnable(GL_DEPTH_TEST);
	
	//Configure depth map FBO
	glGenFramebuffers(1, &depthMapFBO);

	//Create depth texture
	glGenTextures(1, &depthMap_);
	glBindTexture(GL_TEXTURE_2D, depthMap_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Attatch depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
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

void ChunkRenderer::DrawImGui()
{
	ImGui::Begin("Light");
	Vec3f lightDirection = directionalLight_.direction_;
	if (ImGui::InputFloat3("Direction", &lightDirection[0]))
	{
		directionalLight_.direction_ = lightDirection;
	}

	Vec3f lightPosition = directionalLight_.position_;
	if (ImGui::InputFloat3("Position", &lightPosition[0]))
	{
		directionalLight_.position_ = lightPosition;
	}

	ImGui::End();
}

void ChunkRenderer::Update(seconds dt)
{
	GizmosLocator::get().DrawCube(directionalLight_.position_, Vec3f(.5f), Color4(1, 1, 1, 1));
}

void ChunkRenderer::Render()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("ChunkRenderer::Render");
#endif
	const auto& config = BasicEngine::GetInstance()->config;
	

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);


	RenderScene(simpleDepthShader_);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, config.windowSize.x, config.windowSize.y);

	RenderScene(shader_);
	
}
void ChunkRenderer::RenderScene(gl::Shader shader) {
	auto model = Mat4f::Identity;
	const auto lightView = depthCamera_.GenerateViewMatrix();
	const auto lightProjection = depthCamera_.GenerateProjectionMatrix();
	const auto lightSpaceMatrix = lightProjection * lightView;
	shader.Bind();
	shader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);
	
	shader.SetMat4("model", model);
	//SetCameraParameters(camera_);

	shader.SetMat4("view", camera_.GenerateViewMatrix());
	shader.SetMat4("projection", camera_.GenerateProjectionMatrix());
	shader.SetVec3("viewPos", camera_.position);
	shader.SetTexture("shadowMap", depthMap_, 3);
	shader.SetBool("enableBias", true);
	shader.SetBool("enableShadow", enableShadows);
	shader.SetBool("enableOverSampling", true);
	shader.SetBool("enablePcf", true);
	shader.SetVec3("viewPos", camera_.position);
	shader.SetFloat("bias", 0.0005f);
	shader.SetVec3("lightDir", directionalLight_.direction_);
	
	glBindTexture(GL_TEXTURE_2D, atlasTex_);
	const auto visibleChunks = chunkManager_.chunkStatusManager.GetVisibleChunks();
	for (auto& chunk : visibleChunks)
	{
		if (chunkManager_.chunkContentManager.GetChunkSize(chunk) == 0)
			chunkManager_.chunkStatusManager.AddStatus(chunk, ChunkFlag::EMPTY);
		else
			chunkManager_.chunkStatusManager.RemoveStatus(chunk, ChunkFlag::EMPTY);
		if (!chunkManager_.chunkStatusManager.HasStatus(chunk, ChunkFlag::LOADED) ||
			chunkManager_.chunkStatusManager.HasStatus(chunk, ChunkFlag::EMPTY))
			continue;

		shader.SetVec3("chunkPos", Vec3f(chunkManager_.chunkPosManager.GetComponent(chunk)));
		chunkManager_.chunkRenderManager.Draw(chunk);
	}
}

	

void ChunkRenderer::SetCameraParameters(const Camera& camera) const
{
	shader_.SetMat4("view", camera.GenerateViewMatrix());
	shader_.SetMat4("projection", camera.GenerateProjectionMatrix());
	shader_.SetVec3("viewPos", camera.position);
}

void ChunkRenderer::SetLightParameters() const
{
	shader_.SetVec3("light.color", directionalLight_.color_);
	shader_.SetVec3("light.direction", directionalLight_.direction_.Normalized());
	shader_.SetInt("objectMaterial.diffuse", 0);
	shader_.SetInt("objectMaterial.specular", 1);
	shader_.SetInt("objectMaterial.shininess", directionalLight_.specularPow_);

	shader_.SetFloat("ambientStrength", directionalLight_.ambientStrength_);
	shader_.SetFloat("diffuseStrength", directionalLight_.diffuseStrength_);
	shader_.SetFloat("specularStrength", directionalLight_.specularStrength_);
}

void ChunkRenderer::Destroy()
{
	shader_.Destroy();
	gl::DestroyTexture(atlasTex_);
}
}
