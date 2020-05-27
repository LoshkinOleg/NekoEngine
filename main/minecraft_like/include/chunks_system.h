#pragma once
#include <engine/system.h>

namespace neko
{
class ChunksManager;
class MinecraftLikeEngine;

class ChunksSystem final : public SystemInterface
{
public:
	explicit ChunksSystem(MinecraftLikeEngine& engine);

	void Init() override;

	void Update(seconds dt) override;

	void Destroy() override;

private:
	ChunksManager& chunksManager_;
};
}
