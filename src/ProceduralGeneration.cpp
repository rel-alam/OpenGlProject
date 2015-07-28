#include "ProceduralGeneration.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"

#define GSM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include "stb_image.h"


bool ProceduralGeneration::startup()
{
	a_realDims = vec2(128, 128);
	realDims = a_realDims;

	a_dims = glm::ivec2(128, 128);
	dims = a_dims;

	buildGrid(realDims, dims);
	buildPerlinTexture(dims, 8, 0.3f);
	m_scale = 10; 

	loadTexture("./textures/grass.jpg", m_grass_texture);
	loadTexture("./textures/water.jpg", m_water_texture);
	loadTexture("./textures/snow.jpg", m_snow_texture);

	LoadShader("./shaders/perlin_vertex.glsl", 0, "./shaders/perlin_fragment.glsl", &m_program_id);


	m_ambient_light = vec3(0.1f);
	m_light_dir = vec3(0, -1, 0);
	m_light_color = vec3(0.6f, 0, 0);
	m_material_color = vec3(1);
	m_specular_power = 15;

	return true;
}

void ProceduralGeneration::buildPerlinTexture(glm::ivec2 dims, int octaves, float persistance)
{
	//set scale
	float scale = (1.0f / dims.x) * 3.0f;
	//allocate memory for perlin data
	m_perlin_data = new float[dims.x * dims.y];

	//loop through all the pixels
	for (int y = 0; y < dims.y; ++y)
	{
		for (int x = 0; x < dims.x; ++x)
		{
			float amplitude = 1.f;
			
			m_perlin_data[y * dims.x + x] = 0;

			for (int o = 0; o < octaves; ++o)
			{
		//		call glm::perlin function	
				float freq = powf(2, (float)o);
				float perlin_sample = glm::perlin(vec2((float)x, (float)y) * scale * freq) * 0.5f + 0.5f;
				m_perlin_data[y * dims.x + x] += perlin_sample * amplitude;

				amplitude *= persistance;
			}
			
		} 
		
	}
		

//	generate opnegl texture handles
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

//	compute how many indicies we need
	unsigned int index_count = dims.x * dims.y * 6;
//	allocate index data
	unsigned int *index_data = new unsigned int[index_count];

	float curr_y = -real_dims.y / 2.0f;
///	two nested for loops
	for (int y = 0; y < dims.y + 1; ++y)
	{
		float curr_x = -real_dims.x / 2.0f;;
		for (int x = 0; x < dims.x + 1; ++x)
		{
		//	inside we create our points
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
		//	create our 6 indicies here
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


//	unbind stuff
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
	if (realDims != a_realDims)
	{
		realDims = a_realDims;
		buildGrid(realDims, dims);
	}

	if (dims != a_dims)
	{
		dims = a_dims;
		buildPerlinTexture(dims, 8, 0.3f);
	}
	return true;
}

void ProceduralGeneration::draw(FlyCamera a_camera)
{
	glUseProgram(m_program_id);

	int view_proj_uniform = glGetUniformLocation(m_program_id, "view_proj");
	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE, (float*)&a_camera.m_projView);

	int world_uniform = glGetUniformLocation(m_program_id, "world");
	glUniformMatrix4fv(world_uniform, 1, GL_FALSE, (float*)&a_camera.m_world);

	int view_uniform = glGetUniformLocation(m_program_id, "view");
	glUniformMatrix4fv(view_uniform, 1, GL_FALSE, (float*)&a_camera.m_view);

	int tex_uniform = glGetUniformLocation(m_program_id, "perlin_texture");
	glUniform1i(tex_uniform, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);

	int grass_tex_uniform = glGetUniformLocation(m_program_id, "grass_texture");
	glUniform1i(grass_tex_uniform, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_grass_texture);

	int water_tex_uniform = glGetUniformLocation(m_program_id, "water_texture");
	glUniform1i(water_tex_uniform, 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_water_texture);

	int snow_tex_uniform = glGetUniformLocation(m_program_id, "snow_texture");
	glUniform1i(snow_tex_uniform, 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_snow_texture);

	int scale_uniform = glGetUniformLocation(m_program_id, "scale");
	glUniform1f(scale_uniform, m_scale);

	glBindVertexArray(m_plane_mesh.m_VAO);
	glDrawElements(GL_TRIANGLES, m_plane_mesh.m_index_count, GL_UNSIGNED_INT, 0);
}
