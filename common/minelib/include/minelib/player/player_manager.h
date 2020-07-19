#pragma once
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
	
	[[nodiscard]] Player& GetComponent(const Entity entity)
	{ return components_[entity]; }
	
	Entity GetCurrentChunk(Entity playerIndex);
	void SetCurrentChunk(Entity playerIndex, Entity chunkIndex);

private:
	UiManager& uiManager_;
};
}
