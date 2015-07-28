#ifndef _PROCEDURAL_H_
#define _PROCEDURAL_H_

#include "Application.h"
#include "Camera.h"
#include "Utility.h"
#include "Vertex.h"

class ProceduralGeneration
{
public:
	bool startup();
	void shutdown();
	bool update();
	void draw(FlyCamera a_camera);

	void buildGrid(vec2 real_dims, glm::ivec2 dims);
	void buildPerlinTexture(glm::ivec2 dims, int octaves, float persistance);

	OpenGLData m_plane_mesh;
	unsigned int m_program_id;

	unsigned int m_perlin_texture;

	unsigned int m_water_texture;
	unsigned int m_grass_texture;
	unsigned int m_snow_texture;


	float* m_perlin_data;

	float m_scale;
	vec2 realDims;
	glm::ivec2 dims;
	vec2 a_realDims;
	glm::ivec2 a_dims;


	vec3 m_ambient_light;
	vec3 m_light_dir;
	vec3 m_light_color;
	vec3 m_material_color;

	float m_specular_power;


};

#endif // !_PROCEDURAL_H_
