#pragma once
#include <sdl_engine/sdl_engine.h>

#include "blocks/block_manager.h"
#include "components_manager_system.h"
#include "draw_system.h"
#include "ui/ui_manager.h"

namespace neko
{
class MinecraftLikeEngine final : public sdl::SdlEngine
{
public:
	explicit MinecraftLikeEngine(Configuration* config = nullptr);

	void Init() override;

	void Destroy() override;

	void ManageEvent() override;

	void GenerateUiFrame() override;

	//Scene system
	EntityManager entityManager;
	EntityHierarchy entityHierarchy;
	ComponentsManagerSystem componentsManagerSystem;
	BlockManager blockManager;
	UiManager uiManager;
private :
	DrawSystem drawSystem_;

	//Initialization data
	const size_t kInitEntityNmb_ = 10;
};
}
