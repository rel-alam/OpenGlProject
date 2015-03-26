#include "Deferred.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"
#include "tiny_obj_loader.h"

#define GSM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include "Utility.h"

bool Deferred::startup()
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

	buildMesh();
	buildQuad();
	buildCube();
	buildGBuffer();
	buildLightBuffer();
	

	LoadShader("./shaders/gBuffer_vertex.glsl", 0, "./shaders/gBuffer_fragment.glsl", &m_gbuffer_program);

	LoadShader("./shaders/composite_vertex.glsl", 0, "./shaders/composite_fragment.glsl", &m_composite_program);

	LoadShader("./shaders/composite_vertex.glsl", 0, "./shaders/directional_light_fragment.glsl", &m_directional_light_program);

	LoadShader("./shaders/point_light_vertex.glsl", 0, "./shaders/point_light_fragment.glsl", &m_point_light_program);

	glEnable(GL_CULL_FACE);

	return true;
}

void Deferred::shutdown()
{
	Gizmos::destroy();
}
bool Deferred::update()
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

void Deferred::draw()
{

	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, m_gbuffer_fbo);
	

	glClearColor(0, 0, 0, 0);

	glClear( GL_DEPTH_BUFFER_BIT);


	vec4 clear_color = vec4(0, 0, 0, 0);
	vec4 clear_normal = vec4(0.5f, 0.5f, 0.5f, 0.5f);

	glClearBufferfv(GL_COLOR, 0, (float*)&clear_color);
	glClearBufferfv(GL_COLOR, 1, (float*)&clear_color);
	glClearBufferfv(GL_COLOR, 2, (float*)&clear_normal);

	glUseProgram(m_gbuffer_program);

	int view_uniform = glGetUniformLocation(m_gbuffer_program, "view");
	int view_proj_uniform = glGetUniformLocation(m_gbuffer_program, "view_proj");

	glUniformMatrix4fv(view_uniform, 1, GL_FALSE, (float*)&camera.m_view);
	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE, (float*)&camera.m_projView);
	glBindVertexArray(m_bunny.m_VAO);
	glDrawElements(GL_TRIANGLES, m_bunny.m_index_count, GL_UNSIGNED_INT, 0);




	// Light
	glBindFramebuffer(GL_FRAMEBUFFER, m_light_fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glUseProgram(m_directional_light_program);

	int position_tex_uniform = glGetUniformLocation(m_directional_light_program, "position_tex");
	int normals_tex_uniform = glGetUniformLocation(m_directional_light_program, "normals_tex");

	glUniform1i(position_tex_uniform, 0);
	glUniform1i(normals_tex_uniform, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_position_texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normals_texture);


	//renderDirectionalLight(vec3(1, 0, 0), vec3(1, 0, 0));
	//renderDirectionalLight(vec3(0, 1, 0), vec3(0, 1, 0));
	//renderDirectionalLight(vec3(0, 0, 1), vec3(0, 0, 1));

	glUseProgram(m_point_light_program);
	view_proj_uniform = glGetUniformLocation(m_point_light_program, "proj_view");

	position_tex_uniform = glGetUniformLocation(m_point_light_program, "position_texture");

	normals_tex_uniform = glGetUniformLocation(m_point_light_program, "normal_texture");

	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE, (float*)&camera.m_projView);
	glUniform1i(position_tex_uniform, 0);
	glUniform1i(normals_tex_uniform, 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_position_texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normals_texture);

	//draw the point lights
	renderPointLight(vec3(0, 8, 0), 5, vec3(1, 1, 0));
	renderPointLight(vec3(5, 8, 0), 5, vec3(1, 1, 1));
	renderPointLight(vec3(-5, 0, -5), 5, vec3(1, 1, 1));

	glDisable(GL_BLEND);



	// Composite
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.3, 0.3, 0.3, 1);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(m_composite_program);

	int albedo_tex_uniform = glGetUniformLocation(m_composite_program, "albedo_tex");
	int light_tex_uniform = glGetUniformLocation(m_composite_program, "light_tex");

	glUniform1i(albedo_tex_uniform, 0);
	glUniform1i(light_tex_uniform, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_albedo_texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_light_texture);


	glBindVertexArray(m_screenspace_quad.m_VAO);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	

	//Gizmos::draw(camera.getProjectionView());

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}


void Deferred::buildMesh()
{
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	tinyobj::LoadObj(shapes, materials, "./models/stanford/bunny.obj");

	m_bunny.m_index_count = shapes[0].mesh.indices.size();

	tinyobj::mesh_t* mesh = &shapes[0].mesh;

	std::vector<float> vertex_data;
	vertex_data.reserve(mesh->positions.size() + mesh->normals.size());

	vertex_data.insert(vertex_data.end(), mesh->positions.begin(), mesh->positions.end());
	vertex_data.insert(vertex_data.end(), mesh->normals.begin(), mesh->normals.end());

	glGenVertexArrays(1, &m_bunny.m_VAO);
	glBindVertexArray(m_bunny.m_VAO);

	glGenBuffers(1, &m_bunny.m_VBO);
	glGenBuffers(1, &m_bunny.m_IBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_bunny.m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bunny.m_IBO);


	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* vertex_data.size(), vertex_data.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* mesh->indices.size(), mesh->indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float)* mesh->positions.size()));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



}

void Deferred::buildGBuffer()
{
	//create the frame buffer
	glGenFramebuffers(1, &m_gbuffer_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gbuffer_fbo);

	//generate all our textures
	//albedo, position, normal, depth(depth is render buffer)
	glGenTextures(1, &m_albedo_texture);
	glBindTexture(GL_TEXTURE_2D, m_albedo_texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenTextures(1, &m_position_texture);
	glBindTexture(GL_TEXTURE_2D, m_position_texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenTextures(1, &m_normals_texture);
	glBindTexture(GL_TEXTURE_2D, m_normals_texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenRenderbuffers(1, &m_gbuffer_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_gbuffer_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1280, 720);


	// attach our texturs to the frame buffer

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_albedo_texture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_position_texture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, m_normals_texture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_gbuffer_depth);

	GLenum targets[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

	glDrawBuffers(3, targets);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Error in creating gbuffer!\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//check that it worked
}

void Deferred::buildLightBuffer()
{
	//create fbo
	glGenFramebuffers(1, &m_light_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_light_fbo);
	//create textures
	//light texture
	glGenTextures(1, &m_light_texture);
	glBindTexture(GL_TEXTURE_2D, m_light_texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//attach textures
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_light_texture, 0);

	GLenum light_target = GL_COLOR_ATTACHMENT0;
	glDrawBuffers(1, &light_target);

	//check for sucess
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Error - light, framebuffer incorrect /n");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

void Deferred::buildQuad()
{
	vec2 half_texel = 1.0f / vec2(1280, 720) * 0.5f;

	float vertex_data[]
	{
		-1, -1, 0, 1, half_texel.x, half_texel.y,
		 1, -1, 0, 1, 1 - half_texel.x, half_texel.y,
		 1,  1, 0, 1, 1 - half_texel.x, 1 - half_texel.y,
		-1,  1, 0, 1, half_texel.x, 1 - half_texel.y,
	};

	unsigned int index_data[] =
	{
		0, 1, 2, 0, 2, 3
	};

	glGenVertexArrays(1, &m_screenspace_quad.m_VAO);

	glGenBuffers(1, &m_screenspace_quad.m_VBO);
	glGenBuffers(1, &m_screenspace_quad.m_IBO);

	glBindVertexArray(m_screenspace_quad.m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_screenspace_quad.m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_screenspace_quad.m_IBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_data), index_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 6, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 6, (void*)(sizeof(float)* 4));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Deferred::buildCube()
{

	float vertex_data[]
	{
		-1, -1, 1, 1,
		1, -1, 1, 1,
		1, -1, -1, 1,
		-1, -1, -1, 1,

		- 1, 1, 1, 1,
		1, 1, 1, 1,
		1, 1, -1, 1,
		-1, 1, -1, 1

	};

	unsigned int index_data[] =
	{
		4, 5, 0,
		5, 1, 0,
		5, 6, 1,
		6, 2, 1,
		6, 7, 2,
		7, 3, 2,
		7, 4, 3,
		4, 0, 3,
		7, 6, 4,
		6, 5, 4,
		0, 1, 3,
		1, 2, 3,
	};

	glGenVertexArrays(1, &m_light_cube.m_VAO);

	glGenBuffers(1, &m_light_cube.m_VBO);
	glGenBuffers(1, &m_light_cube.m_IBO);

	glBindVertexArray(m_light_cube.m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_light_cube.m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_light_cube.m_IBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_data), index_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //position

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 4, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void Deferred::renderDirectionalLight(vec3 light_dir, vec3 light_color)
{
	//vec3 light_dir = vec3(0, -1, 0);
	vec4 viewspace_light_dir = camera.m_view * vec4(light_dir, 0);
	//vec3 light_color = vec3(1, 0, 1);

	int light_dir_uniform = glGetUniformLocation(m_directional_light_program, "light_dir");
	int light_color_uniform = glGetUniformLocation(m_directional_light_program, "light_color");

	glUniform3fv(light_dir_uniform, 1, (float*)&viewspace_light_dir);
	glUniform3fv(light_color_uniform, 1, (float*)&light_color);

	glBindVertexArray(m_screenspace_quad.m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Deferred::renderPointLight(vec3 position, float radius, vec3 light_color)
{
	vec4 view_space_pos = camera.m_view * vec4(position, 1);

	int pos_uniform = glGetUniformLocation(m_point_light_program, "light_position");
	int view_pos_uniform = glGetUniformLocation(m_point_light_program, "light_view_position");
	int light_diffuse_uniform = glGetUniformLocation(m_point_light_program, "light_diffuse");
	int light_radius_uniform = glGetUniformLocation(m_point_light_program, "light_radius");

	glUniform3fv(pos_uniform, 1, (float*)&position);
	glUniform3fv(view_pos_uniform, 1, (float*)&view_space_pos);
	glUniform3fv(light_diffuse_uniform, 1, (float*)&light_color);
	glUniform1f(light_radius_uniform, radius);

	glBindVertexArray(m_light_cube.m_VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}