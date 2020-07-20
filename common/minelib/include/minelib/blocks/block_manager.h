#pragma once
#include "engine/system.h"

#include "gl/shader.h"
#include "gl/shape.h"
#include "graphics/camera.h"
#include "graphics/graphics.h"

#include "minelib/blocks/block.h"

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
	void InitRegister();

	void Update(seconds dt) override {}
	void FixedUpdate() override {}

	void Render() override;

	void RegisterBlock(Block& block);
	
	std::shared_ptr<Block> GetBlock(const size_t blockIndex)
	{ return std::make_shared<Block>(registeredBlocks_[blockIndex]); }
	std::shared_ptr<Block> GetRandomBlock()
	{ return std::make_shared<Block>(registeredBlocks_[RandomRange(1, registeredBlocks_.size())]); }

	void Destroy() override;

private:
	Camera2D previewCam_;
	unsigned previewFbo_ = 0;
	unsigned previewRbo_ = 0;
	size_t previewTexSize_ = 32;
	TextureId atlas_ = 0;

	std::vector<Block> registeredBlocks_{};

	gl::Shader previewShader_;
	gl::RenderCuboid cube_{Vec3f::zero, Vec3f::one};
};

using BlockManagerLocator = Locator<IBlockManager, NullBlockManager>;
}
