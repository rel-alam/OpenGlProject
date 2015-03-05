#include "BoundingSphere.h"

BoundingSphere::BoundingSphere() : centre(0), radius(0)
{
}

BoundingSphere::~BoundingSphere()
{

}

void BoundingSphere::fit(std::vector<vec3> points)
{
	vec3 min(1e37f);
	vec3 max(-1e37f);

	for (auto p : points)
	{
		if (p.x < min.x) min.x = p.x;
		if (p.y < min.y) min.y = p.y;
		if (p.z < min.z) min.y = p.z;
		if (p.x > max.x) max.y = p.x;
		if (p.y > max.y) max.y = p.y;
		if (p.z > max.z) max.y = p.z;
	}

	centre = (min + max) * 0.5f;
	radius = glm::distance(min, centre);
}