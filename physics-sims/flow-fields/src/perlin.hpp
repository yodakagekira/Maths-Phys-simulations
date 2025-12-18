#pragma once
#include <cstdint>

namespace noise {

// Classic 3D Perlin noise in [-1, 1]
float perlin3(float x, float y, float z);

// Fractal Brownian Motion built from perlin3, also roughly in [-1, 1]
float fbm3(float x, float y, float z, int octaves = 5, float lacunarity = 2.0f, float gain = 0.5f);
} 