#pragma once
#include <engine/system.h>

#include <graphics/camera.h>
#include <graphics/graphics.h>
#include <utilities/service_locator.h>

#include <block.h>

namespace neko
{

const static size_t kMaxBlocksNum = 16;
//-----------------------------------------------------------------------------
// BlockManagerInterface
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
class IBlockManager
{
protected:
	~IBlockManager() = default;
};

//-----------------------------------------------------------------------------
// NullBlockManager
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
class NullBlockManager final : public IBlockManager
{
};

//-----------------------------------------------------------------------------
// BlockManager
//-----------------------------------------------------------------------------
/// \brief Draw gizmos
class BlockManager final : public RenderCommandInterface,
                             public SystemInterface,
                             public IBlockManager
{
public:
	BlockManager();

	void Init() override;

	void Update(seconds dt) override {}
	void FixedUpdate() override {}

	void Render() override;

	void RegisterBlock(BaseBlock* block);
	
	BaseBlock* GetBlock(const size_t blockIndex)
	{ return registeredBlocks_[blockIndex]; }

	void Destroy() override;

private:
	std::mutex updateMutex_;

	Camera2D previewCam_;
	unsigned previewFbo_ = 0;
	unsigned previewRbo_ = 0;
	size_t previewTexSize_ = 32;
	gl::Shader previewShader_;

	std::vector<BaseBlock*> registeredBlocks_{};

	gl::Shader testShader_;
	gl::RenderQuad testQuad_{Vec3f::zero, Vec2f::one};
};

using BlockManagerLocator = Locator<IBlockManager, NullBlockManager>;
}
