#pragma once
#include <mathematics/vector.h>

namespace neko
{

using Color3 = Vec3f;
using Color4 = Vec4f;


const static Color4 white = Color4(1.0f, 1.0f, 1.0f, 1.0f);
const static Color4 grey = Color4(0.5f, 0.5f, 0.5f, 1.0f);
const static Color4 gray = grey;
const static Color4 black = Color4(0.0f, 0.0f, 0.0f, 1.0f);

const static Color4 red = Color4(1.0f, 0.0f, 0.0f, 1.0f);
const static Color4 green = Color4(0.0f, 1.0f, 0.0f, 1.0f);
const static Color4 blue = Color4(0.0f, 0.0f, 1.0f, 1.0f);

const static Color4 yellow = Color4(1.0f, 1.0f, 0.0f, 1.0f);
const static Color4 cyan = Color4(0.0f, 1.0f, 1.0f, 1.0f);
const static Color4 magenta = Color4(1.0f, 0.0f, 1.0f, 1.0f);

const static Color4 clear = Color4(0.0f, 0.0f, 0.0f, 0.0f);


}
