#pragma once
#include <vector>

class PerlinNoise
{
public:
	// Function to linearly interpolate between a0 and a1
	// Weight w should be in the range [0.0, 1.0]
	float Lerp(float a0, float a1, float w)
	{
		return (1.0 - w) * a0 + w * a1;
	}

	//Computes the dot product of the distance and gradient vectors
	float DotGridGradient(int ix, int iy, float x, float y)
	{
		// Precomputed (or otherwise) gradient vectors at eaxh grid node
		extern float gradient[IYMAX][IXMAX][2];

		//Compute the distance vector
		float dx = x - (float)ix;
		float dy = y - (float)iy;

		//Compute the dot-product
		return (dx * gradient[iy][ix][0] + dy*gradient[iy][ix][1]);
	}

	//Compute Perlin noise at coordinates x, y
	float Perlin(float x, float y)
	{
		//Determine grid cell coordinates
		int x0 = (int)x;
		int x1 = x0 + 1;
		int y0 = (int)y;
		int y1 = y0 + 1;

		//Determine interpolation weights
		//Could also use higher order polynomial/s-curve here
		float sx = x - (float)x0;
		float sy = y - (float)y0;

		//Interpolate between grid point gradients
		float n0, n1, ix0, ix1, value;

		n0 = DotGridGradient(x0, y0, x, y);
		n1 = DotGridGradient(x1, y0, x, y);
		ix0 = Lerp(n0, n1, sx);

		n0 = DotGridGradient(x0, y1, x, y);
		n1 = DotGridGradient(x1, y1, x, y);
		ix1 = Lerp(n0, n1, sx);

		value = Lerp(ix0, ix1, sy);
		return value;
	}

};
