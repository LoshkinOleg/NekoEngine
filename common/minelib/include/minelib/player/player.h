#pragma once
#include "minelib/ui/ui_element.h"

namespace neko
{
const static short kHotBarSize = 9;

struct HotBarTile
{
	Block& block;
	UiElement blockPreview;
};

struct PlayerUi
{
	PlayerUi& operator=(const PlayerUi& other);
	
	//Crosshair
	UiElement crossHair{Vec3f::zero, Vec2u(64, 64)};
	
	//Crosshair
	UiElement hotBar{Vec3f::zero, Vec2u(728, 88)};
	std::array<HotBarTile, kHotBarSize> hotBarContent{};
	
	//Block Select
	UiElement blockSelect{Vec3f::zero, Vec2u(96, 96)};
	short selectIndex = 0;
};

struct Player
{
	Player& operator=(const Player& other);
	
    FpsCamera camera;
	Vec3f position = Vec3f::zero;
	
	Entity currentChunk = INVALID_ENTITY;

	PlayerUi ui;
	
	//Player variables
    float maxReach = 8.0f;
	float placeCoolDown = 0.2f;
	float placeTimeStamp = 0.0f;
};
}
