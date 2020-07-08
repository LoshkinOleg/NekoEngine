#pragma once
#include "minelib/blocks/block.h"
#include "minelib/ui/ui_manager.h"

namespace neko
{
struct Player
{
	enum PlayerState : uint8_t
	{
		CAN_JUMP = 1u << 0u,
		SPRINTING = 1u << 1u,
		FLYING = 1u << 2u,
	};
	
	//Player variables
	Vec3f pos = Vec3f::back * 3 + Vec3f::up * 2;
	Vec3f aabbSize = Vec3f(0.5f, 0.9f, 0.5f);
	Aabb3d aabb;
	uint8_t state = 0;
    float maxReach = 6.0f;
	float placeCoolDown = 0.2f;
	float placeTimeStamp = 0.0f;

	//Movement variables
	Vec3f velocity = Vec3f::zero;
	float decelerationSpeed = 9.0f;
	float sprintMultiplier = 1.3f;
	float speedMultiplier = 1.0f;
	
	//Jump Variables
	float jumpPower = 9.8f;

	//Head Bobbing
	Vec3f cameraOffset = Vec3f::up * 1.5f;
    float bobbingSpeed = 14.0f;
    float bobbingAmount = 0.05f;
    float defaultPosY = 1.5f;
    float bobbingTimer = 0;
};

class PlayerController final : SystemInterface
{
public:
	explicit PlayerController(MinecraftLikeEngine& engine);
	
    void Init() override;
    void Update(seconds dt) override;
	void FixedUpdate() override;
    void Destroy() override;

	void PlaceCube(size_t chunkId, size_t blockId) const;
	void DeleteCube(size_t chunkId, size_t blockId) const;

	void MovePlayer();
	
	void ResolvePhysics();
	
	void HeadBobbing();
	
	void CheckBlock(const uint16_t blockId, const Vec3f& blockPos) const;

    bool IsMoving() const;
private:
	float gravity_ = 36.0f;
    PlayerCamera camera_;

	Player player_;

	MinecraftLikeEngine& engine_;
	sdl::IInputManager& inputManager_;
	IGizmosRenderer& gizmosRenderer_;
	ChunksManager& chunksManager_;
	IAabbManager& aabbManager_;
	IUiManager& uiManager_;
	
	//UI
	//Crosshair
	UiElement crossHair_{Vec3f::zero, Vec2u(64, 64)};
	
	//Toolbar
	UiElement toolBar_{Vec3f::zero, Vec2u(728, 88)};
	const static short toolbarSize = 9;
	std::array<Block*, toolbarSize> toolBarBlocks_{nullptr};
	
	//Block previews
	std::array<UiElement, toolbarSize> blockPreviews_{};
	
	//Block Select
	UiElement blockSelect_{Vec3f::zero, Vec2u(96, 96)};
	short selectIndex_ = 0;
	
	Vec2u tileSize_ = Vec2u(80u);
};
}