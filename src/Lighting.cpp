#include "Lighting.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"

#define GSM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "Utility.h"
#include "FBXFile.h"

bool Lighting::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
	glClearColor(0.3f, 0.3f, 0.3f, 1);
	glEnable(GL_DEPTH_TEST);



	Gizmos::create();

	camera = new FlyCamera();
	camera->setLookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	camera->setSpeed(1);
	camera->setPrespective(60, 1280 / 720, 0.1f, 1000.f);

	m_fbx_file = new FBXFile();
	m_fbx_file->load("./models/stanford/Bunny.fbx");

	createOpenGLBuffers(m_fbx_file);
	LoadShader("./shaders/lighting_vertex.glsl", "./shaders/lighting_fragment.glsl", &m_program_id);

	return true;
}

void Lighting::reloadShader()
{
	glDeleteProgram(m_program_id);
	LoadShader("./shaders/lighting_vertex.glsl", "./shaders/lighting_fragment.glsl", &m_program_id);
}

void Lighting::shutdown()
{

	cleanUpOpenGLBuffers(m_fbx_file);


	Gizmos::destroy();
	Application::shutdown();
}
bool Lighting::update()
{
	if (Application::update() == false)
	{
		return false;
	}


	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	camera->update(dt);

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

	if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS)
	{
		reloadShader();
	}

	return true;
}

void Lighting::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_program_id);

	int proj_view_uniform = glGetUniformLocation(m_program_id, "projection_view");
	glUniformMatrix4fv(proj_view_uniform, 1, GL_FALSE, (float*)&camera->getProjectionView());

	for (unsigned int mesh_index = 0; mesh_index < m_fbx_file->getMeshCount(); ++mesh_index)
	{
		FBXMeshNode* mesh = m_fbx_file->getMeshByIndex(mesh_index);

		unsigned int *gl_data = ((unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}


	Gizmos::draw(camera->getProjectionView());

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void Lighting::createOpenGLBuffers(FBXFile* file)
{
	for (unsigned int mesh_index = 0; mesh_index < file->getMeshCount(); ++mesh_index)
	{
		FBXMeshNode* mesh = file->getMeshByIndex(mesh_index);

		//contains [0] is VAO, [1] is VBO and [2] is IBO
		unsigned int* gl_data = new unsigned int[3];


		glGenVertexArrays(1, &gl_data[0]);
		glGenBuffers(2, &gl_data[1]);

		glBindVertexArray(gl_data[0]);

		glBindBuffer(GL_ARRAY_BUFFER, gl_data[1]);
		glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(FBXVertex), mesh->m_vertices.data(), GL_STATIC_DRAW);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_data[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_vertices.size()* sizeof(unsigned int), mesh->m_vertices.data(), GL_STATIC_DRAW);


		glEnableVertexAttribArray(0); //position
		glEnableVertexAttribArray(1); //normal data

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), (void*)(FBXVertex::NormalOffset));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		mesh->m_userData = gl_data;

	}
}

void Lighting::cleanUpOpenGLBuffers(FBXFile* file)
{
	for (unsigned int mesh_index = 0; mesh_index < file->getMeshCount(); ++mesh_index)
	{
		FBXMeshNode* mesh = file->getMeshByIndex(mesh_index);

		unsigned int * gl_data = (unsigned int*)mesh->m_userData;

		glDeleteVertexArrays(1, &gl_data[0]);
		glDeleteBuffers(2, &gl_data[1]);

		delete[] gl_data;
	}
}
