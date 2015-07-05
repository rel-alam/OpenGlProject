#include "ProceduralGeneration.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"

#define GSM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"

bool ProceduralGeneration::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
	m_draw_gizmos = true;

	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(1280, 720);

	glClearColor(0.3f, 0.3f, 0.3f, 1);
	glEnable(GL_DEPTH_TEST);

	glfwSetMouseButtonCallback(m_window, OnMouseButton);
	glfwSetCursorPosCallback(m_window, OnMousePosition);
	glfwSetScrollCallback(m_window, OnMouseScroll);
	glfwSetKeyCallback(m_window, OnKey);
	glfwSetCharCallback(m_window, OnChar);
	glfwSetWindowSizeCallback(m_window, OnWindowResize);



	Gizmos::create();

	camera = FlyCamera();
	camera.setLookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	camera.setSpeed(40);
	camera.setPrespective(60, 1280 / 720, 0.1f, 1000.f);

	buildGrid(vec2(64, 64), glm::ivec2(64, 64));
	buildPerlinTexture(glm::ivec2(64, 64), 5, 0.3f);
	m_scale = 3; 


	m_bar = TwNewBar("Control Panel");

	TwAddVarRW(m_bar, "Set Scale", TW_TYPE_FLOAT, &m_scale, "");

	TwAddVarRW(m_bar, "Draw Gizmos", TW_TYPE_BOOL8, &m_draw_gizmos, "");
	TwAddVarRO(m_bar, "FPS", TW_TYPE_FLOAT, &m_fps, "");




	LoadShader("./data/shaders/perlin_vertex.glsl", 0, "./data/shaders/perlin_fragment.glsl", &m_program_id);

	return true;
}

void ProceduralGeneration::buildPerlinTexture(glm::ivec2 dims, int octaves, float persistance)
{
	//set scale
	float scale = (1.0f / dims.x) * 5.0f;
	//allocate memory for perlin data
	m_perlin_data = new float[dims.x * dims.y];

	//loop through all the pixels
	for (int y = 0; y < dims.y; ++y)
	{
		for (int x = 0; x < dims.x; ++x)
		{
			float amplitude = 1;
			float freq = 1;
			m_perlin_data[y * dims.x + x] = 0;

			for (int o = 0; o < octaves; ++o)
			{
				//call glm::perlin function	
				float perlin_sample = glm::perlin(vec2(x, y) * scale * freq) * 0.5f + 0.5f;
				perlin_sample *= amplitude;
				m_perlin_data[y * dims.x + x] += perlin_sample;

				amplitude *= persistance;
				freq *= 2.0f;
			}
			
		} 
		
	}
		

	//generate opnegl texture handles
	glGenTextures(1, &m_perlin_texture);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);

	//pass perlin data to use the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, dims.x, dims.y, 0, GL_RED, GL_FLOAT, m_perlin_data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);

	
}

void ProceduralGeneration::buildGrid(vec2 real_dims, glm::ivec2 dims)
{
	//compute how many verticies needed.
	unsigned int vertex_count = (dims.x + 1) * (dims.y + 1);
	//allocate vertex data
	VertexTexCoord* vertex_data = new VertexTexCoord[vertex_count];

	//compute how many indicies we need
	unsigned int index_count = dims.x * dims.y * 6;
	//allocate index data
	unsigned int *index_data = new unsigned int[index_count];

	float curr_y = -real_dims.y / 2.0f;
	//two nested for loops
	for (int y = 0; y < dims.y + 1; ++y)
	{
		float curr_x = -real_dims.x / 2.0f;;
		for (int x = 0; x < dims.x + 1; ++x)
		{
			//inside we create our points
			vertex_data[y * (dims.x + 1) + x].position = vec4(curr_x, 0, curr_y, 1);
			vertex_data[y * (dims.x + 1) + x].tex_coord = vec2((float)x / (float)dims.x, (float)y / (float)dims.y);

			curr_x += real_dims.x / (float)dims.x;
		}
		curr_y += real_dims.y / (float)dims.y;
	}
		
	//generate index data
	int curr_index = 0;
	for (int y = 0; y < dims.y; ++y)
	{
		for (int x = 0; x < dims.x; ++x)
		{
			//create our 6 indicies here
			index_data[curr_index++] = y * (dims.x + 1) + x;
			index_data[curr_index++] = (y + 1) * (dims.x + 1) + x;
			index_data[curr_index++] = (y + 1) * (dims.x + 1) + (x + 1);

			index_data[curr_index++] = (y + 1) * (dims.x + 1) + (x + 1);
			index_data[curr_index++] = y * (dims.x + 1) + (x + 1);
			index_data[curr_index++] = y * (dims.x + 1) + x;
		}
	}

	m_plane_mesh.m_index_count = index_count;

	//create vertexarrayobject, buffers, etc
	glGenVertexArrays(1, &m_plane_mesh.m_VAO);
	glGenBuffers(1, &m_plane_mesh.m_VBO);
	glGenBuffers(1, &m_plane_mesh.m_IBO);

	//bind and fill buffers
	glBindVertexArray(m_plane_mesh.m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_plane_mesh.m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_plane_mesh.m_IBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexTexCoord)* vertex_count, vertex_data, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* index_count, index_data, GL_STATIC_DRAW);

	//tell opengl about our vertex structure
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexTexCoord), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTexCoord), (void*)sizeof(vec4));


	//unbind stuff
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//free vertex and index data
	delete[] vertex_data;
	delete[] index_data;



}

void ProceduralGeneration::shutdown()
{
	Gizmos::destroy();
}
bool ProceduralGeneration::update()
{
	if (Application::update() == false)
	{
		return false;
	}
	Gizmos::clear();


	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	m_fps = 1 / dt;


	if (glfwGetKey(m_window, GLFW_KEY_Z))
	{
		m_scale += 5 * dt;
	}
	if (glfwGetKey(m_window, GLFW_KEY_X))
	{
		m_scale -= 5 * dt;
	}


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

void ProceduralGeneration::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_program_id);

	int view_proj_uniform = glGetUniformLocation(m_program_id, "view_proj");
	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE, (float*)&camera.m_projView);

	int tex_uniform = glGetUniformLocation(m_program_id, "perlin_texture");
	glUniform1i(tex_uniform, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);

	int scale_uniform = glGetUniformLocation(m_program_id, "scale");
	glUniform1f(scale_uniform, m_scale);

	glBindVertexArray(m_plane_mesh.m_VAO);
	glDrawElements(GL_TRIANGLES, m_plane_mesh.m_index_count, GL_UNSIGNED_INT, 0);

	if (m_draw_gizmos)
	{
		Gizmos::draw(camera.getProjectionView());
	}

	TwDraw();
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}
