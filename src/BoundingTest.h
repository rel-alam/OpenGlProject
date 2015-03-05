#ifndef _BOUND_TEST_H
#define _BOUND_TEST_H

#include "Application.h"
#include "Camera.h"
#include "BoundingSphere.h"
#include "AABB.h"
class BoundingTest : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();

	virtual bool update();
	virtual void draw();

	void getFrustrumPlanes(mat4 transform, vec4* planes);



	FlyCamera camera;

	vec4 plane;
	vec4 planes[6];
	BoundingSphere sphere;

};

#endif // !_BOUND_TEST_H
