#include "minecraft_like_engine.h"
#ifdef EASY_PROFILE_USE
#include <easy/profiler.h>
#endif

namespace neko
{
MinecraftLikeEngine::MinecraftLikeEngine(Configuration* config)
	: SdlEngine(config),
	  entityManager_(),
	  entityHierarchy_(entityManager_),
	  componentsManagerSystem_(*this),
	  drawSystem_(*this),
	  light_(*this)
{
	RegisterSystem(drawSystem_);
	RegisterOnEvent(drawSystem_);
	RegisterOnDrawUi(drawSystem_);
	RegisterSystem(componentsManagerSystem_);
#ifdef EASY_PROFILE_USE
	EASY_PROFILER_ENABLE;
#endif

}

void MinecraftLikeEngine::Init()
{
	SdlEngine::Init();
	//Temporary scene init
	for (int i = 0; i < kInitEntityNmb_; i++)
	{
		const auto entity = entityManager_.CreateEntity();
		componentsManagerSystem_.transform3dManager_.AddComponent(entity);
		/*componentsManagerSystem_.
			transform3dManager_.
			SetPosition(entity,
			            Vec3f(RandomRange(-10.0f, 10.0f),
			                  RandomRange(-10.0f, 10.0f),
			                  RandomRange(0.0f, 10.0f)));
		componentsManagerSystem_.transform3dManager_.SetScale(
			entity,
			Vec3f::one * RandomRange(0.5f, 3.0f));
		componentsManagerSystem_.
			transform3dManager_.
			SetRotation(entity,
			            EulerAngles(degree_t(RandomRange(0.0f, 90.0f)),
			                        degree_t(RandomRange(0.0f, 90.0f)),
			                        degree_t(RandomRange(0.0f, 90.0f))));*/
	}
	entityManager_.AddComponentType(0, EntityMask(ComponentType::CHUNK));
	componentsManagerSystem_.chunkManager_.AddComponent(0);
}

void MinecraftLikeEngine::Destroy()
{
#ifdef EASY_PROFILE_USE
	profiler::dumpBlocksToFile("minecraft_profile.prof");
#endif
	drawSystem_.Destroy();
	SdlEngine::Destroy();
}

void MinecraftLikeEngine::ManageEvent()
{
	SdlEngine::ManageEvent();
}

void MinecraftLikeEngine::GenerateUiFrame()
{
	SdlEngine::GenerateUiFrame();
}
}