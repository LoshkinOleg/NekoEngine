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
ChunkRenderer::ChunkRenderer(MinecraftLikeEngine& engine, FpsCamera* camera)
	: camera_(camera),
	  engine_(engine),
	  chunkManager_(engine.componentsManagerSystem.chunkManager)
{
}

void ChunkRenderer::Init()
{
	stbi_set_flip_vertically_on_load(true);
	const auto& config = BasicEngine::GetInstance()->config;

	//Load Shaders
	shader_.LoadFromFile(
		config.dataRootPath + "shaders/minecraft_like/cubeShadow.vert",
		config.dataRootPath + "shaders/minecraft_like/cubeShadow.frag");
	simpleDepthShader_.LoadFromFile(
		config.dataRootPath + "shaders/minecraft_like/shadowMappingDepth.vert",
		config.dataRootPath + "shaders/minecraft_like/shadowMappingDepth.frag");
	skyboxCube_.Init();
	skyboxTexture_ = gl::LoadCubemap({
		config.dataRootPath + "sprites/skybox_bluesky/right.png",
		config.dataRootPath + "sprites/skybox_bluesky/left.png",
		config.dataRootPath + "sprites/skybox_bluesky/top.png",
		config.dataRootPath + "sprites/skybox_bluesky/bottom.png",
		config.dataRootPath + "sprites/skybox_bluesky/front.png",
		config.dataRootPath + "sprites/skybox_bluesky/back.png"
		});
	skyboxCube_.SetTexture(skyboxTexture_);
	
	//Load Textures
	atlasTex_ = gl::stbCreateTexture(config.dataRootPath + "sprites/atlas.png", gl::Texture::CLAMP_WRAP);
	
	InitShadow();

	//Set Light values
	directionalLight_.position = Vec3f::up * 50.0f;
	directionalLight_.direction = -Vec3f::one;

	//Set depthCamera values
	depthCamera_.SetSize(Vec2f::one * 50.0f);
	depthCamera_.nearPlane = 0.1f;
	depthCamera_.position = directionalLight_.position;
	depthCamera_.reverseDirection = -directionalLight_.direction.Normalized();

}

void ChunkRenderer::DrawImGui()
{
	ImGui::Begin("Light");
	Vec3f lightDirection = directionalLight_.direction;
	if (ImGui::DragFloat3("Direction", &lightDirection[0], 0.01f, -1.0f, 1.0f))
	{
		directionalLight_.direction = lightDirection;
		depthCamera_.reverseDirection = -lightDirection;
	}

	Vec3f lightPosition = directionalLight_.position;
	if (ImGui::DragFloat3("Position", &lightPosition[0], 0.01f))
	{
		directionalLight_.position = lightPosition;
		//depthCamera_.position = lightPosition;
	}

	Vec3f depthPosition = depthCamera_.position;
	if (ImGui::DragFloat3("Depth Position", &depthPosition[0], 0.01f))
	{
		depthCamera_.position = depthPosition;
	}

	bool shadowToggle = enableShadow;
	if(ImGui::Checkbox("Enable Shadows", &shadowToggle)) {
		enableShadow = shadowToggle;
	}
	
	auto size = depthCamera_.GetSize();
	if (ImGui::DragFloat2("Size", &size[0], 0.01f))
	{
		depthCamera_.SetSize(size);
	}

	auto color = directionalLight_.color;
	if (ImGui::DragFloat3("Light Color", &color[0], 0.1f))
	{
		directionalLight_.color = color;
	}

	float intensity = directionalLight_.intensity;
	if (ImGui::DragFloat("Light Intensity", &intensity, 0.1f))
	{
		directionalLight_.intensity = intensity;
	}

	ImGui::End();
}

void ChunkRenderer::Update(seconds dt)
{
	depthCamera_.WorldLookAt(-depthCamera_.reverseDirection + depthCamera_.position);
	directionalLight_.position = camera_->position - directionalLight_.direction * 50.0f;
	depthCamera_.position = directionalLight_.position;
		

#ifdef EASY_PROFILE_USE
	EASY_BLOCK("ChunkRenderer::Update", profiler::colors::Green);
#endif
	GizmosLocator::get().DrawCube(directionalLight_.position, Vec3f(.5f), Color4(1, 1, 1, 1));
}

void ChunkRenderer::Render()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("ChunkRenderer::Render", profiler::colors::DeepOrange);
#endif
	const auto& config = BasicEngine::GetInstance()->config;

	//Render With shadows
	glEnable(GL_CULL_FACE);
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);
	SetLightParameters();
	RenderScene(simpleDepthShader_);
	
	//Render normal
	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, config.windowSize.x, config.windowSize.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RenderScene(shader_);

	skyboxCube_.Draw(camera_->GenerateViewMatrix(), camera_->GenerateProjectionMatrix());
}

	
void ChunkRenderer::RenderScene(gl::Shader& shader) const
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("ChunkRenderer::RenderScene", profiler::colors::DeepOrange100);
#endif
	SetCameraParameters(*camera_, shader);
	
	SetShadowParameters(shader);

//	glBindTexture(GL_TEXTURE_2D, atlasTex_);
	const auto renderedChunks = chunkManager_.chunkStatusManager.GetRenderedChunks();
	for (auto& chunk : renderedChunks)
	{		
		shader_.SetVec3("chunkPos", Vec3f(chunkManager_.chunkPosManager.GetPositon(chunk)));
		chunkManager_.chunkRenderManager.Draw(chunk);
	}
}


void ChunkRenderer::InitShadow() {
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
}

	
void ChunkRenderer::SetCameraParameters(const Camera& camera, gl::Shader& shader) const
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("ChunkRenderer::SetCameraParameters", profiler::colors::DeepOrange200);
#endif
	shader.Bind();
	shader.SetMat4("view", camera.GenerateViewMatrix());
	shader.SetMat4("projection", camera.GenerateProjectionMatrix());
	shader.SetVec3("viewPos", camera.position);
}

void ChunkRenderer::SetShadowParameters(gl::Shader& shader) const {
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("ChunkRenderer::SetShadowParameters", profiler::colors::DeepOrange200);
#endif
	const auto lightView = depthCamera_.GenerateViewMatrix();
	const auto lightProjection = depthCamera_.GenerateProjectionMatrix();
	const auto lightSpaceMatrix = lightProjection * lightView;
	
	shader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);
	shader.SetTexture("material.diffuse", atlasTex_, 0);
	shader.SetTexture("shadowMap", depthMap_, 1);
	shader.SetVec3("light.direction", directionalLight_.direction.Normalized());
	shader.SetFloat("bias", bias_);
	shader.SetBool("enableShadow", enableShadow);

	shader.SetVec3("light.color", directionalLight_.color);
	shader.SetFloat("light.intensity", directionalLight_.intensity);

}


void ChunkRenderer::SetLightParameters() const
{


}

void ChunkRenderer::SetCamera(FpsCamera* camera)
{
	camera_ = camera;
}

void ChunkRenderer::Destroy()
{
	shader_.Destroy();
	gl::DestroyTexture(atlasTex_);
}
}
