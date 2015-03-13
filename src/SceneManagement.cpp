#include "SceneManagement.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"
#include "tiny_obj_loader.h"
#include "Utility.h"

#define GSM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"

void RenderPlane(vec4 plane)
{
	vec3 up = vec3(0, 1, 0);
	if (plane.xyz() == vec3(0, 1, 0))
	{
		up = vec3(1, 0, 0);
	}
	vec3 tangent = glm::normalize(glm::cross(plane.xyz(), up));
	vec3 bitangent = glm::normalize(glm::cross(plane.xyz(), tangent));

	vec3 p = plane.xyz() * plane.w;

	vec3 v0 = p + tangent + bitangent;
	vec3 v1 = p + tangent - bitangent;
	vec3 v2 = p - tangent - bitangent;
	vec3 v3 = p - tangent + bitangent;
	
	Gizmos::addTri(v0, v1, v1, vec4(1, 1, 0, 1));
	Gizmos::addTri(v0, v2, v3, vec4(1, 1, 0, 1));

	Gizmos::addLine(p, p + plane.xyz() * 2, vec4(0, 1, 1, 1));
}

AABB GenerateAABB(vec3* positions, unsigned int count, unsigned int stride = 0)
{
	AABB result = {};
	if (stride == 0)
	{
		stride = sizeof(vec3);
	}
	
	result.min = positions[0];
	result.max = positions[0];

	for (unsigned int pos_index = 0; pos_index < count; ++pos_index)
	{
		if (positions->x < result.min.x) result.min.x = positions->x;
		if (positions->y < result.min.y) result.min.y = positions->y;
		if (positions->z < result.min.z) result.min.z = positions->z;

		if (positions->x > result.max.x) result.max.x = positions->x;
		if (positions->y > result.max.y) result.max.y = positions->y;
		if (positions->z > result.max.z) result.max.z = positions->z;

		positions = (vec3*)((char*)positions + stride);
	}
	return result;

}

void RenderAABB(AABB aabb, mat4 transform)
{
	vec3 center = ((aabb.min + transform[3].xyz()) + (aabb.max + transform[3].xyz())) * 0.5f;
	vec3 extents = ((aabb.max + transform[3].xyz()) + (aabb.min + transform[3].xyz())) * 0.5f;

	Gizmos::addAABB(center, extents, vec4(1, 1, 1, 1), &transform);
}

void GetFrustumPlanes(mat4 view_proj, vec4* planes)
{
	//right side
	planes[0] = vec4(view_proj[0][3] - view_proj[1][0],
		view_proj[1][3] - view_proj[1][0],
		view_proj[2][3] - view_proj[2][0],
		view_proj[3][3] - view_proj[3][0]);
	// left side
	planes[1] = vec4(view_proj[0][3] + view_proj[0][0],
		view_proj[1][3] + view_proj[1][0],
		view_proj[2][3] + view_proj[2][0],
		view_proj[3][3] + view_proj[3][0]);
	// top
	planes[2] = vec4(view_proj[0][3] - view_proj[0][1],
		view_proj[1][3] - view_proj[1][1],
		view_proj[2][3] - view_proj[2][1],
		view_proj[3][3] - view_proj[3][1]);
	// bottom
	planes[3] = vec4(view_proj[0][3] + view_proj[0][1],
		view_proj[1][3] + view_proj[1][1],
		view_proj[2][3] + view_proj[2][1],
		view_proj[3][3] + view_proj[3][1]);
	// far
	planes[4] = vec4(view_proj[0][3] - view_proj[0][2],
		view_proj[1][3] - view_proj[1][2],
		view_proj[2][3] - view_proj[2][2],
		view_proj[3][3] - view_proj[3][2]);
	// near
	planes[5] = vec4(view_proj[0][3] + view_proj[0][2],
		view_proj[1][3] + view_proj[1][2],
		view_proj[2][3] + view_proj[2][2],
		view_proj[3][3] + view_proj[3][2]);

	for (int i = 0; i < 6; i++)
	{
		planes[i] = glm::normalize(planes[i]);
	}

}

bool OnPositivePlaneSide(vec4 plane, AABB aabb, mat4 transform)
{
	return false;
}

bool SceneManagement::startup()
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
	return true;
}

void SceneManagement::shutdown()
{
	Gizmos::destroy();
}
bool SceneManagement::update()
{
	if (Application::update() == false)
	{
		return false;
	}
	Gizmos::clear();


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

	return true;
}

void SceneManagement::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(camera.getProjectionView());

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}