#include "RenderTargets.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"
#include "Utility.h"
#define GSM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"

bool RenderTargets::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
	glClearColor(0.3f, 0.3f, 0.3f, 1);
	glEnable(GL_DEPTH_TEST);



	Gizmos::create();
	generateFrameBuffer();
	generatePlane();
	LoadShader("./shaders/textured_vertex.glsl", 0, "./shaders/textured_fragment.glsl", &m_program_id);


	camera = FlyCamera();
	camera.setLookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	camera.setSpeed(40);
	camera.setPrespective(60, 1280 / 720, 0.1f, 1000.f);
	return true;
}

void RenderTargets::shutdown()
{
	Gizmos::destroy();
}
bool RenderTargets::update()
{
	if (Application::update() == false)
	{
		return false;
	}
	Gizmos::clear();

	Gizmos::addSphere(vec3(0, 5, 0), 0.5f, 12, 12, vec4(1, 1, 0, 1));

	Gizmos::addSphere(vec3(0, 10, 0), 0.5f, 12, 12, vec4(1, 0, 0, 1));

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

void RenderTargets::generateFrameBuffer()
{
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	glGenTextures(1, &m_fbo_texture);
	glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 512, 512);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fbo_texture, 0);

	glGenRenderbuffers(1, &m_fbo_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fbo_depth);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Framebuffer Did not generate properly!\n");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void RenderTargets::generatePlane()
{
	float vertex_data[] = {
		-5, 0, -5, 1, 0, 0,
		5, 0, -5, 1, 1, 0,
		5, 10, -5, 1, 1, 1,
		-5, 10, -5, 1, 0, 1,

	};
	unsigned int index_data[] = {
		0, 1, 2,
		0, 2, 3,
	};

	m_plane.m_index_count = 6;

	glGenVertexArrays(1, &m_plane.m_VAO);
	glBindVertexArray(m_plane.m_VAO);
	glGenBuffers(1, &m_plane.m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_plane.m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, vertex_data, GL_STATIC_DRAW);

	glGenBuffers(1, &m_plane.m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_plane.m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* 6, index_data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 6, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 6, ((char*)0) + 16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void RenderTargets::draw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, 512, 512);
	glClearColor(0.75f, 0.75f, 0.75f, 1);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	mat4 world = camera.m_world;
	vec4 plane = vec4(0, 0, 1, -5);
	vec3 reflected = glm::reflect(-world[2].xyz(), plane.xyz());
	reflected = glm::normalize(reflected);

	float dist = (glm::dot((plane.xyz() * plane.w) - world[3].xyz(), plane.xyz())) / (glm::dot(plane.xyz(), -world[2].xyz()));

	vec3 inter = world[3].xyz()+ (- world[2].xyz()) * dist;
	world[3].xyz = inter - reflected *dist;

	vec3 up = vec3(0, 1, 0);

	world[2].xyz = -reflected;
	world[0].xyz = glm::normalize(glm::cross(world[2].xyz(), up));
	world[1].xyz = glm::normalize(glm::cross(world[0].xyz(), world[2].xyz()));

	mat4 view = glm::inverse(world);
	mat4 proj_view = camera.m_proj * view;


	glUseProgram(m_program_id);

	int proj_view_uniform = glGetUniformLocation(m_program_id, "projection_view");
	glUniformMatrix4fv(proj_view_uniform, 1, GL_FALSE, (float*)&(proj_view));

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	glBindVertexArray(m_plane.m_VAO);
	glDrawElements(GL_TRIANGLES, m_plane.m_index_count, GL_UNSIGNED_INT, 0);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glDepthFunc(GL_GREATER);
	Gizmos::draw(proj_view);
	glDepthFunc(GL_LESS);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);
	glClearColor(0.25f, 0.25f, 0.25f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	Gizmos::draw(camera.getProjectionView());
	glUseProgram(m_program_id);

	int proj_view_ref_uniform = glGetUniformLocation(m_program_id, "projection_view_reflected");
	glUniformMatrix4fv(proj_view_ref_uniform, 1, GL_FALSE, &(camera.getProjectionView()[0][0]));



	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
	glUniform1i(glGetUniformLocation(m_program_id, "diffuse"), 0);

	glBindVertexArray(m_plane.m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}