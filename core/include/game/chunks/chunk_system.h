#pragma once
#include <vector>
#include "../../../../common/gles3_wrapper/include/gl/shape.h"
#include "../../mathematics/vector.h"

namespace neko::game
{
class Chunk;

class ChunkSystem
{
public:
	void UpdateChunk(const Vec3i& pos);
	void DrawChunk(const Vec3i& pos);

	void OnPlayerChangeChunk(const Vec3i& pos);
	
	void DrawCube(const Vec3f& cubePos);
private:
	std::vector<Chunk> chunks;
	
    gl::RenderCuboid cube_{Vec3f::zero, Vec3f::one};
};
}
