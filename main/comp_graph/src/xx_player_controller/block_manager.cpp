#include <xx_player_controller/block_manager.h>

namespace neko
{
BlockManager::BlockManager()
{
	BlockManagerLocator::provide(this);

	registeredBlocks_.reserve(kMaxBlocksNum);
	
	const auto& config = BasicEngine::GetInstance()->config;
	RegisterBlock(new Block("Dirt", config.dataRootPath + "sprites/blocks/dirt.png"));

	const std::array<std::string, 3> grassCubeTex
	{
		config.dataRootPath + "sprites/blocks/grass_side.png",
		config.dataRootPath + "sprites/blocks/grass_top.png",
		config.dataRootPath + "sprites/blocks/dirt.png"
	};
	RegisterBlock(new UniqueBlock("Grass", grassCubeTex));

	RegisterBlock(new Block("Stone", config.dataRootPath + "sprites/blocks/stone.jpg"));
	RegisterBlock(new Block("Diamond Ore", config.dataRootPath + "sprites/blocks/diamond_ore.jpg"));
}

void BlockManager::Init()
{
	const auto& config = BasicEngine::GetInstance()->config;
	previewShader_.LoadFromFile(
		config.dataRootPath + "shaders/xx_player_controller/base_model.vert",
		config.dataRootPath + "shaders/xx_player_controller/base_model.frag");

    testShader_.LoadFromFile(
            config.dataRootPath+"shaders/11_hello_framebuffer/screen.vert",
            config.dataRootPath+"shaders/11_hello_framebuffer/screen.frag"
            );
	testQuad_.Init();
	
	previewShader_.Bind();
	previewShader_.SetMat4("model", Mat4f::Identity);
	
	glGenFramebuffers(1, &previewFbo_);
	glBindFramebuffer(GL_FRAMEBUFFER, previewFbo_);

	glViewport(0, 0, previewTexSize_, previewTexSize_);
	glGenRenderbuffers(1, &previewRbo_);
	glBindRenderbuffer(GL_RENDERBUFFER, previewRbo_);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, previewTexSize_, previewTexSize_);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	for (auto& block : registeredBlocks_)
	{
		block->Init();
		
		glGenTextures(1, &block->previewTexture);
		glBindTexture(GL_TEXTURE_2D, block->previewTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, previewTexSize_, previewTexSize_, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, block->previewTexture, 0);
		
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, previewRbo_);
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
		    LogDebug("[Error] Framebuffer is not complete!");
		}
		
		previewShader_.Bind();
		Mat4f model = Transform3d::Rotate(Mat4f::Identity, 
			EulerAngles(degree_t(25.0f), degree_t(45.0f), degree_t(0.0f)));
		model = Transform3d::Scale(model, Vec3f(1, -1, 1));
		previewShader_.SetMat4("model", model);
		
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		block->Draw();
	}
	glViewport(0, 0, config.windowSize.x, config.windowSize.y);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BlockManager::Render()
{
	/*testShader_.Bind();
    testShader_.SetInt("screenTexture", 0);
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, registeredBlocks_[0]->previewTexture);
	testQuad_.Draw();
    glEnable(GL_DEPTH_TEST);*/
}

void BlockManager::RegisterBlock(BaseBlock* block)
{
	registeredBlocks_.push_back(block);
	block->id = registeredBlocks_.size() - 1;
}

void BlockManager::Destroy()
{
	for (auto& block : registeredBlocks_)
	{
		block->Destroy();
		delete block;
	}
	registeredBlocks_.clear();
    glDeleteFramebuffers(1, &previewFbo_);
}
}
