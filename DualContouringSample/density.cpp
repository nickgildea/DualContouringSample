#include "density.h"

#include <glm/ext.hpp>
using namespace glm;

// ----------------------------------------------------------------------------

float Sphere(const vec3& worldPosition, const vec3& origin, float radius)
{
	return length(worldPosition - origin) - radius;
}

// ----------------------------------------------------------------------------

float Cuboid(const vec3& worldPosition, const vec3& origin, const vec3& halfDimensions)
{
	const vec3& local_pos = worldPosition - origin;
	const vec3& pos = local_pos;

	const vec3& d = glm::abs(pos) - halfDimensions;
	const float m = max(d.x, max(d.y, d.z));
	return min(m, length(max(d, vec3(0.f))));
}

// ----------------------------------------------------------------------------

float FractalNoise(
	const int octaves,
	const float frequency,
	const float lacunarity,
	const float persistence,
	const vec2& position)
{
	const float SCALE = 1.f / 128.f;
	vec2 p = position * SCALE;
	float noise = 0.f;

	float amplitude = 1.f;
	p *= frequency;

	for (int i = 0; i < octaves; i++)
	{
		noise += simplex(p) * amplitude;
		p *= lacunarity;	
		amplitude *= persistence;	
	}

	// move into [0, 1] range
	return 0.5f + (0.5f * noise);
}

// ----------------------------------------------------------------------------

float Density_Func(const vec3& worldPosition)
{
	const float MAX_HEIGHT = 20.f;
	const float noise = FractalNoise(4, 0.5343f, 2.2324f, 0.68324f, vec2(worldPosition.x, worldPosition.z));
	const float terrain = worldPosition.y - (MAX_HEIGHT * noise);

	const float cube = Cuboid(worldPosition, vec3(-4., 10.f, -4.f), vec3(12.f));
	const float sphere = Sphere(worldPosition, vec3(15.f, 2.5f, 1.f), 16.f);

	return max(-cube, min(sphere, terrain));
}
