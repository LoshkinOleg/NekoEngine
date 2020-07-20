#pragma once
#include "minelib/blocks/block.h"
#include "minelib/ui/ui_manager.h"
#include "player_manager.h"

namespace neko
{
class PlayerController final : public SystemInterface, public sdl::SdlEventSystemInterface
{
public:
	explicit PlayerController(MinecraftLikeEngine& engine);
	
    void Init() override;
    void Update(seconds dt) override;
	void UpdateUi();
	void FixedUpdate() override;
	void OnEvent(const SDL_Event& event) override;
    void Destroy() override;

	void PlaceBlock(Entity chunk, BlockId blockId, const Block& block) const;
	void BreakBlock(Entity chunk, BlockId blockId) const;

	void SetCurrentPlayer(Entity player);
	Entity GetCurrentPlayer() const;
	
private:
	Entity currentPlayer_ = INVALID_ENTITY;
	
	MinecraftLikeEngine& engine_;
	EntityManager& entityManager_;
	BlockManager& blockManager_;
	IUiManager& uiManager_;
	
	PlayerManager& playerManager_;
	ChunkManager& chunkManager_;
	IAabbManager& aabbManager_;
	
	sdl::IInputManager& inputManager_;
	IGizmosRenderer& gizmosRenderer_;
	
	PlayerUi ui_;
};
}
