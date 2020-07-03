#pragma once
#include <mathematics/vector.h>

#include "gl/shader.h"
#include "graphics/color.h"

namespace neko
{
struct DirectionalLight {
	Vec3f position_;
	Vec3f direction_ = Vec3f(-1.0f, -1.0f, 0.0f);
	Color3 color_{ 1,1,1 };
	
	float ambientStrength_ = 0.1f;
	float diffuseStrength_ = 1.0f;
	float specularStrength_ = 0.5f;
	int specularPow_ = 32;
};
};
