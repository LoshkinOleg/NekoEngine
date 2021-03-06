#pragma once
#include <engine\entity.h>
#include "engine/component.h"

#include "minelib/ui/ui_manager.h"
#include "player.h"

namespace neko
{
class PlayerManager final : public ComponentManager<Player, ComponentType::PLAYER>
{
	using ComponentManager::ComponentManager;
public:
	explicit PlayerManager(EntityManager& entityManager, UiManager& uiManager);;
	
	Index AddComponent(Entity playerIndex) override;
	
	FpsCamera* GetCamera(const Entity playerIndex)
	{ return &components_[playerIndex].camera; }
	
	Entity GetCurrentChunk(Entity playerIndex);
	void SetCurrentChunk(Entity playerIndex, Entity chunkIndex);

private:
	UiManager& uiManager_;
};
}
