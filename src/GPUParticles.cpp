#include "GPUParticles.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"

#define GSM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"

bool GPUParticles::startup()
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
	
	
	m_emitter.Init(10000, vec3(0, 0, 0), 0.5, 5, 5, 0.8f, 1.0f, 1, 0.01f, vec4(1, 0, 0, 1), vec4(1, 0.4f, 0, 1));
	m_time = 0;

	return true;
}

void GPUParticles::shutdown()
{
	Gizmos::destroy();
}
bool GPUParticles::update()
{
	if (Application::update() == false)
	{
		return false;
	}


	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);
	m_time += dt;
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

void GPUParticles::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(camera.getProjectionView());
	m_emitter.Draw(m_time, camera.getWorldTransform(), camera.getProjectionView());
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}