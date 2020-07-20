#include "minelib/player/player_manager.h"

namespace neko
{
PlayerManager::PlayerManager(EntityManager& entityManager, UiManager& uiManager)
	: ComponentManager<Player, ComponentType::PLAYER>(entityManager),
	  uiManager_(uiManager)
{
}

Index PlayerManager::AddComponent(const Entity playerIndex)
{
    ResizeIfNecessary(components_, playerIndex, Player());
	entityManager_.AddComponentType(playerIndex, static_cast<EntityMask>(ComponentType::PLAYER));
	
	components_[playerIndex].camera.Init();
	
    return playerIndex;
}

Entity PlayerManager::GetCurrentChunk(const Entity playerIndex)
{
	return components_[playerIndex].currentChunk;
}

void PlayerManager::SetCurrentChunk(const Entity playerIndex, const Entity chunkIndex)
{
	components_[playerIndex].currentChunk = chunkIndex;
}
}
