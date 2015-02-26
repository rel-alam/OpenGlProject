#include "Particles.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"
#include "Utility.h"

#define GSM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"

bool Particles::startup()
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

	LoadShader("./shaders/particle_vertex.glsl", "./shaders/particle_fragment.glsl", &m_program_id);

	m_emitter.Init(100000, vec3(0, 0, 0), 500, 1, 3, 0.5f, 3.0f, 0.05, 0.005f, vec4(1, 0.4f, 0, 1), vec4(1, 0, 0, 1));

	

	return true;
}

void Particles::shutdown()
{
	Gizmos::destroy();
}
bool Particles::update()
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

	m_emitter.Update(dt);
	m_emitter.UpdateVertexData(camera.m_world[3].xyz, camera.m_world[1].xyz);

	return true;
}

void Particles::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(camera.getProjectionView());
	mat4 pv = camera.getProjectionView();
	glUseProgram(m_program_id);
	int proj_view_uniform = glGetUniformLocation(m_program_id, "projection_view");
	glUniformMatrix4fv(proj_view_uniform, 1, GL_FALSE, (float*)&camera.m_projView);

	m_emitter.Render();

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}