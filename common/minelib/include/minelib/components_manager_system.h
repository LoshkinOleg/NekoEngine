#pragma once
#include <engine/transform.h>

#include "minelib/aabb_manager.h"
#include "minelib/chunks/chunk_manager.h"
#include "minelib/chunks/chunk_system.h"

namespace neko
{
class MinecraftLikeEngine;

class ComponentsManagerSystem final : public SystemInterface
{
public:
	explicit ComponentsManagerSystem(MinecraftLikeEngine& engine);

	void Init() override;

	void Update(seconds dt) override;
	void FixedUpdate() override {}

	void Destroy() override;

	Transform3dManager transform3dManager_;
	ChunksManager chunkManager_;
protected:
	ChunksSystem chunksSystem_;
	AabbManager aabbManager_;

};
}
