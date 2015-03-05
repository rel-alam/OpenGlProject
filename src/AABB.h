#ifndef _AABB_H_
#define _AABB_H_

#include "gl_core_4_4.h"
#include "glm_header.h"
#include <vector>


class AABB
{
public:
	AABB();
	~AABB();

	void reset();
	void fit(std::vector<vec3> points);

	vec3 min;
	vec3 max;
};
#endif // !_AABB_H_
