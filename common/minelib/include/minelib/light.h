#pragma once
#include <mathematics/vector.h>

#include "graphics/color.h"

namespace neko
{
struct DirectionalLight
{
	Vec3f position = Vec3f::one * 10.0f;
	Vec3f direction = Vec3f(-1.0f, -1.0f, 0.0f);
	Color3 color = Vec3f(1.0f, 1.0f, 1.0f);
	
	float intensity = 2.0f;
	float ambientStrength = 1.0f;
	float diffuseStrength = 1.0f;
	float specularStrength = 1.0f;
	int specularPow = 32;
};
};
