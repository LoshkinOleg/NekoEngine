#pragma once
#include <engine/transform.h>

#include "aabb_manager.h"
#include "chunks_manager.h"
#include "chunks_system.h"

namespace neko
{
class MinecraftLikeEngine;

class ComponentsManagerSystem final : public SystemInterface
{
public:
	explicit ComponentsManagerSystem(MinecraftLikeEngine& engine);

	void Init() override;

	void Update(seconds dt) override;

	void Destroy() override;

	Transform3dManager transform3dManager_;
	ChunksManager chunkManager_;
protected:
	ChunksSystem chunksSystem_;
	AabbManager aabbManager_;

};
}
