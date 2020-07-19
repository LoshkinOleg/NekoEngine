#include "minelib/player/player.h"

namespace neko
{
Player& Player::operator=(const Player& other)
{
	camera.position = other.camera.position;
	camera.reverseDirection = other.camera.reverseDirection;
	
	currentChunk = other.currentChunk;
	maxReach = other.maxReach;
	placeCoolDown = other.placeCoolDown;
	placeTimeStamp = other.placeTimeStamp;

	return *this;
}
}
