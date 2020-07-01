#pragma once

#include <cmath>

namespace neko
{
inline float Abs(const float v)
{
	return std::abs(v);
}

template <typename T>
float Sign(const T v)
{
	return (v > 0) ? 1 : (v == 0) ? 0 : -1;
}

inline bool Equal(const float a, const float b, const float epsilon = 0.0000001f)
{
	return Abs(a - b) < epsilon;
}
	
inline float Lerp(float start, float end, float t)
{
	return start + (end - start) * t;
}

float RandomRange(float start, float end);

static float Lerp(const float v1, const float v2, const float t)
{
    return v1 + (v2 - v1) * t;
}
}
