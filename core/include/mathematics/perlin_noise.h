#pragma once
#include <random>
#include <vector>



class PerlinNoise
{
public:
	explicit PerlinNoise(std::uint32_t seed = std::default_random_engine::default_seed)
	{
		reseed(seed);
	}

	void reseed(std::uint32_t seed)
	{
		for (size_t i = 0; i < 256; ++i)
		{
			p[i] = static_cast<std::uint8_t>(i);
		}

		std::shuffle(std::begin(p), std::begin(p) + 256, std::default_random_engine(seed));

		for (size_t i = 0; i < 256; ++i)
		{
			p[256 + i] = p[i];
		}
	}

	float noise1D(float x) const 
	{
		return noise3D(x, 0, 0);
	}

	float noise2D(float x, float y) const 
	{
		return noise3D(x, y, 0);
	}
	
	float noise3D(float x, float y, float z) const 
	{
		const std::int32_t X = static_cast<std::int32_t>(std::floor(x)) & 255;
		const std::int32_t Y = static_cast<std::int32_t>(std::floor(y)) & 255;
		const std::int32_t Z = static_cast<std::int32_t>(std::floor(z)) & 255;

		x -= std::floor(x);
		y -= std::floor(y);
		z -= std::floor(z);

		const float u = Fade(x);
		const float v = Fade(y);
		const float w = Fade(z);

		const std::int32_t A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
		const std::int32_t B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

		return Lerp(w, Lerp(v, Lerp(u, Grad(p[AA], x, y, z),
			Grad(p[BA], x - 1, y, z)),
			Lerp(u, Grad(p[AB], x, y - 1, z),
				Grad(p[BB], x - 1, y - 1, z))),
			Lerp(v, Lerp(u, Grad(p[AA + 1], x, y, z - 1),
				Grad(p[BA + 1], x - 1, y, z - 1)),
				Lerp(u, Grad(p[AB + 1], x, y - 1, z - 1),
					Grad(p[BB + 1], x - 1, y - 1, z - 1))));
	}
	
private:
	std::uint8_t p[512];
	
	static const float Fade(float t)
	{
		return t * t * t * (t * (t * 6 - 15) + 10);		//TODO: Remove magic numbers
	}

	static const float Lerp(float t, float a, float b)
	{
		return a + t * (b - a);
	}

 	static const float Grad(std::uint8_t hash, float x, float y, float z)
 	{
 		const std::uint8_t h = hash & 15;
 		const float u = h < 8 ? x : y;
		const float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
 	}

	static const float Weight(std::int32_t octaves)
	{
		float amp = 1;
		float value = 0;

		for (std::int32_t i = 0; i < octaves; i++)
		{
			value += amp;
			amp /= 2;
		}

		return value;
	}
};



/*class PerlinNoise
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
		// Precomputed (or otherwise) gradient vectors at each grid node
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

};*/
