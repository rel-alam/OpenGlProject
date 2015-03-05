#include "AABB.h"

AABB::AABB()
{
	reset();
}

AABB::~AABB()
{

}

void AABB::reset()
{
	min.x = min.y = min.z = 1e37f;
	max.x = max.y = max.z = -1e37f;
}

void AABB::fit(std::vector<vec3> points)
{
	for (auto p : points)
	{
		if (p.x < min.x) min.x = p.x;
		if (p.y < min.y) min.y = p.y;
		if (p.z < min.z) min.y = p.z;
		if (p.x > max.x) max.y = p.x;
		if (p.y > max.y) max.y = p.y;
		if (p.z > max.z) max.y = p.z;
	}
}