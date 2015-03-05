#include "BoundingTest.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"

#define GSM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"

bool BoundingTest::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
	glClearColor(0.3f, 0.3f, 0.3f, 1);
	glEnable(GL_DEPTH_TEST);



	Gizmos::create();

	camera = FlyCamera();
	camera.setLookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	camera.setSpeed(40);
	camera.setPrespective(60, 1280 / 720, 0.1f, 1000.f);

	plane = vec4(0, 1, 0, -1);
	sphere.centre = vec3(0);
	sphere.radius = 0.5f;
	return true;
}

void BoundingTest::shutdown()
{
	Gizmos::destroy();
}
bool BoundingTest::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	camera.update(dt);

	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	vec4 green(0, 1, 0, 1);
	vec4 red(1, 0, 0, 1);
	vec4 blue(0, 0, 1, 1);
	vec4 yellow(1, 1, 0, 1);

	for (int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10), i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i), i == 10 ? white : black);
	}

	float d = glm::dot(vec3(plane), sphere.centre) + plane.w;
//	if (d < sphere.radius)
//		printf("Back\n");
//
//	else if (d > sphere.radius)
//		printf("Front\n");
//
//	else
//		printf("On The Plane\n");

	Gizmos::addSphere(sphere.centre, sphere.radius, 8, 8, vec4(1, 0, 1, 1));
	getFrustrumPlanes(camera.getProjectionView(), planes);
	for (int i = 0; i < 6; i++)
	{
		float d = glm::dot(vec3(planes[i]), sphere.centre) + planes[i].w;
		if (d< -sphere.radius)
		{
			printf("Behind, Don't render it!\n");
			break;
		}
		else if (d<sphere.radius)
		{
			printf("Touching, we still need to render it!\n");
		}
		else
		{
			printf("Front, fully visible so render it!\n");
		}
	}
	vec4 planeColor(1, 1, 0, 1);
	if (d > sphere.radius)
		planeColor = vec4(0, 1, 0, 1);
	else if (d < -sphere.radius)
		planeColor = vec4(1, 0, 0, 1);

	Gizmos::addTri(vec3(4, 1, 4), vec3(-4, 1, -4), vec3(-4, 1, 4), planeColor);
	Gizmos::addTri(vec3(4, 1, 4), vec3(4, 1, -4), vec3(-4, 1, -4), planeColor);

	

	return true;
}

void BoundingTest::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(camera.getProjectionView());

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}


void BoundingTest::getFrustrumPlanes(mat4 transform, vec4* planes)
{
	planes[0] = vec4(transform[0][3] - transform[1][0],
		transform[1][3] - transform[1][0],
		transform[2][3] - transform[2][0],
		transform[3][3] - transform[3][0]
		);
	// left side
	planes[1] = vec4(transform[0][3] + transform[0][0],
		transform[1][3] + transform[1][0],
		transform[2][3] + transform[2][0],
		transform[3][3] + transform[3][0]);
	// top
	planes[2] = vec4(transform[0][3] - transform[0][1],
		transform[1][3] - transform[1][1],
		transform[2][3] - transform[2][1],
		transform[3][3] - transform[3][1]);
	// bottom
	planes[3] = vec4(transform[0][3] + transform[0][1],
		transform[1][3] + transform[1][1],
		transform[2][3] + transform[2][1],
		transform[3][3] + transform[3][1]);
	// far
	planes[4] = vec4(transform[0][3] - transform[0][2],
		transform[1][3] - transform[1][2],
		transform[2][3] - transform[2][2],
		transform[3][3] - transform[3][2]);
	// near
	planes[5] = vec4(transform[0][3] + transform[0][2],
		transform[1][3] + transform[1][2],
		transform[2][3] + transform[2][2],
		transform[3][3] + transform[3][2]);

	for (int i = 0; i < 6; i++)
	{
		planes[i] = glm::normalize(planes[i]);
	}

}