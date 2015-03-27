#include "PerlinNoise.h"

PerlinNoise::PerlinNoise()
{
	for (int x = 0; x < 512; x++)
	{
		p[x] = permutation[x % 256];
	}
	repeat = -1;
}

double PerlinNoise::perlin(double x, double y, double z)
{
	if (repeat > 0)
	{
		x = x % repeat;
		y = y % repeat;
		z = z % repeat;
	}
}