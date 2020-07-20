#include "minelib/player/player.h"

namespace neko
{
void PlayerUi::Update()
{
	const auto& inputManager = sdl::InputLocator::get();
	const auto scrollAmount = inputManager.GetMouseScroll();
	if (scrollAmount.y != 0)
	{
		selectIndex += -Sign(scrollAmount.y);
		if (selectIndex > kHotBarSize - 1) selectIndex = 0;
		if (selectIndex < 0) selectIndex = kHotBarSize - 1;
		
		const auto& config = BasicEngine::GetInstance()->config;
		const Vec2f tileSize = Vec2f(kTileSize) / Vec2f(config.windowSize);
		blockSelect.position.x = hotBar.position.x + (selectIndex - 4) * tileSize.x;
		blockSelect.position.y = hotBar.position.y;
		blockSelect.Update(config.windowSize);
	}
}

PlayerUi& PlayerUi::operator=(const PlayerUi& other)
{
	crossHair = other.crossHair;
	hotBar = other.hotBar;
	hotBarBlocks = other.hotBarBlocks;
	hotBarPreviews = other.hotBarPreviews;
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

	return *this;
}
}
