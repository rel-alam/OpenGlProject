#include "PostProcess.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"
#include "Utility.h"

#define GSM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"


void PostProcess::generateFrameBuffer()
{
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	glGenTextures(1, &m_fbo_texture);
	glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fbo_texture, 0);

	glGenRenderbuffers(1, &m_fbo_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1280, 720);

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

void PostProcess::generateScreenspaceQuad()
{
	vec2 half_texel = 1.0f / vec2(1280, 720) * 0.5f;
	float vertexData[] = {
		-1, -1, 0, 1, half_texel.x, half_texel.y,
		 1,	 1, 0, 1, 1 - half_texel.x, 1 - half_texel.y,
		-1,	 1, 0, 1, half_texel.x, 1 - half_texel.y,
		-1, -1, 0, 1, half_texel.x, half_texel.y,
		 1,	-1, 0, 1, 1 - half_texel.x, half_texel.y,
		 1,	 1, 0, 1, 1 - half_texel.x, 1 - half_texel.y,

	};

	glGenVertexArrays(1, &m_quad.m_VAO);
	glBindVertexArray(m_quad.m_VAO);

	glGenBuffers(1, &m_quad.m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_quad.m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 6 * 6, vertexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 6, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 6, ((char*)0) + 16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool PostProcess::startup()
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

	generateFrameBuffer();
	generateScreenspaceQuad();
	LoadShader("./shaders/post_vertex.glsl", 0, "./shaders/post_fragment.glsl", &m_program_id);

	return true;
}

void PostProcess::shutdown()
{
	Gizmos::destroy();
}
bool PostProcess::update()
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
	Gizmos::addSphere(vec3(0, 0, 0), 4.f, 25, 25, vec4(1, 0.5, 0, 1));

	return true;
}

void PostProcess::draw()
{

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, 1280, 720);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(camera.getProjectionView());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);

	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_program_id);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fbo_texture);

	int loc = glGetUniformLocation(m_program_id, "target"); 
	glUniform1i(loc, 0);

	glBindVertexArray(m_quad.m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}