#pragma once
#include "minelib/ui/ui_element.h"
#include <minelib\blocks\block.h>
#include <graphics\camera.h>

namespace neko
{
const static uint8_t kHotBarSize = 9;
const static Vec2u kTileSize = Vec2u(80u);

struct PlayerUi
{
	void Update();
	
	PlayerUi& operator=(const PlayerUi& other);
	
	//Crosshair
	UiElement crossHair{Vec3f::zero, Vec2u(64, 64)};
	
	//HotBar
	UiElement hotBar{Vec3f::zero, Vec2u(728, 88)};
	std::array<size_t, kHotBarSize> hotBarBlocks{};
	std::array<UiElement, kHotBarSize> hotBarPreviews{};
	
	//Block Select
	UiElement blockSelect{Vec3f::zero, Vec2u(96, 96)};
	short selectIndex = 0;
};

struct Player
{
	Player();
	Player& operator=(const Player& other);
	
    FpsCamera camera;
	Vec3f position = Vec3f::zero;
	
	Entity currentChunk = INVALID_ENTITY;
	
	//Player variables
    float maxReach = 8.0f;
	float placeCoolDown = 0.2f;
	float placeTimeStamp = 0.0f;
};
}
