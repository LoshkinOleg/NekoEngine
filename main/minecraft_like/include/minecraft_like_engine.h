#pragma once
#include <sdl_engine/sdl_engine.h>

#include "components_manager_system.h"
#include "draw_system.h"

namespace neko
{
class MinecraftLikeEngine : public sdl::SdlEngine
{
public:
	MinecraftLikeEngine(Configuration* config = nullptr);

	void Init() override;

	void Destroy() override;

	void ManageEvent() override;

	void GenerateUiFrame() override;

	//Scene system
	EntityManager entityManager_;
	EntityHierarchy entityHierarchy_;
	ComponentsManagerSystem componentsManagerSystem_;

private :
	DrawSystem drawSystem_;

	//Initialization data
	const size_t kInitEntityNmb_ = 10;
};
}
