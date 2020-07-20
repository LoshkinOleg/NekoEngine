#include "minelib/player/player.h"

namespace neko
{
PlayerUi::PlayerUi()
{
}

PlayerUi& PlayerUi::operator=(const PlayerUi& other)
{
	crossHair = other.crossHair;
	hotBar = other.hotBar;
	blockSelect = other.blockSelect;
	selectIndex = other.selectIndex;
	hotBarContent = other.hotBarContent;

	return *this;
}

Player::Player()
{
}

Player& Player::operator=(const Player& other)
{
	camera = other.camera;

	position = other.position;
	currentChunk = other.currentChunk;
	maxReach = other.maxReach;
	placeCoolDown = other.placeCoolDown;
	placeTimeStamp = other.placeTimeStamp;

	ui = other.ui;

	return *this;
}
}
