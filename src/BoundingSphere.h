#ifndef _BOUNDING_SPHERE_H_
#define _BOUNDING_SPHERE_H_

#include "gl_core_4_4.h"
#include "glm_header.h"
#include <vector>

class BoundingSphere
{
public:

	BoundingSphere();
	~BoundingSphere();

	void fit(std::vector<vec3> points);

	vec3 centre;
	float radius;
};

#endif // !_BOUNDING_SPHERE_H_
