#pragma once
#include "minelib/ui/ui_element.h"

namespace neko
{
struct PlayerUi
{
	//Crosshair
	UiElement crossHair{Vec3f::zero, Vec2u(64, 64)};
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
