#include "minelib/blocks/block_manager.h"

#include "gl/texture.h"

namespace neko
{
BlockManager::BlockManager()
{
	BlockManagerLocator::provide(this);

	registeredBlocks_.reserve(kMaxBlocksNum);
}

void BlockManager::Init()
{
	const auto& config = BasicEngine::GetInstance()->config;

	InitRegister();
	previewShader_.LoadFromFile(
		config.dataRootPath + "shaders/minecraft_like/base/preview_block.vert",
		config.dataRootPath + "shaders/minecraft_like/base/preview_block.frag");
	atlas_ = gl::stbCreateTexture(config.dataRootPath + "sprites/atlas.png", gl::Texture::CLAMP_WRAP);
	cube_.Init();
	
	previewShader_.Bind();
	previewShader_.SetMat4("model", Mat4f::Identity);
	
	glGenFramebuffers(1, &previewFbo_);
	glBindFramebuffer(GL_FRAMEBUFFER, previewFbo_);

	glViewport(0, 0, previewTexSize_, previewTexSize_);
	glGenRenderbuffers(1, &previewRbo_);
	glBindRenderbuffer(GL_RENDERBUFFER, previewRbo_);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, previewTexSize_, previewTexSize_);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
		
	previewShader_.Bind();
	Mat4f model = Transform3d::Rotate(Mat4f::Identity, 
		EulerAngles(degree_t(25.0f), degree_t(45.0f), degree_t(0.0f)));
	model = Transform3d::Scale(model, Vec3f(1, -1, 1));
	previewShader_.SetMat4("model", model);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	for (auto& block : registeredBlocks_)
	{
		glGenTextures(1, &block.previewTexture);
		glBindTexture(GL_TEXTURE_2D, block.previewTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, previewTexSize_, previewTexSize_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, block.previewTexture, 0);
		
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, previewRbo_);
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
		    LogDebug("[Error] Framebuffer is not complete!");
		}
		
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_2D, atlas_);
		previewShader_.SetUInt("sideTexId", block.blockTex.sideTexId);
		previewShader_.SetUInt("topTexId", block.blockTex.topTexId);
		previewShader_.SetUInt("bottomTexId", block.blockTex.bottomTexId);
		cube_.Draw();
	}
	glViewport(0, 0, config.windowSize.x, config.windowSize.y);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BlockManager::InitRegister()
{
	RegisterBlock(Block("Air"));
	RegisterBlock(Block("Stone", 1));
	RegisterBlock(Block("Dirt", 2));
	RegisterBlock(Block("Grass", 3, 4, 2));
	RegisterBlock(Block("Iron Ore", 5));
	RegisterBlock(Block("Diamond Ore", 6));
	RegisterBlock(Block("Glass", 7));
}

void BlockManager::Render()
{
}

void BlockManager::RegisterBlock(const Block& block)
{
	registeredBlocks_.push_back(block);
	registeredBlocks_.back().id = registeredBlocks_.size() - 1;
}

Block& BlockManager::GetBlockByTexHash(const TextureHash blockTex)
{
	const auto it = std::find_if(registeredBlocks_.begin(), registeredBlocks_.end(),
		[blockTex](const Block& content)
		{ return BlockTexToTexHash(content.blockTex) == blockTex; });
	if (it != registeredBlocks_.end())
	{
		return registeredBlocks_[it - registeredBlocks_.begin()];
	}

	return registeredBlocks_[0];
}

void BlockManager::Destroy()
{
	cube_.Destroy();
	previewShader_.Destroy();
	registeredBlocks_.clear();
    glDeleteFramebuffers(1, &previewFbo_);
}
}
