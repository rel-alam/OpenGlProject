#ifndef _PROCEDURAL_H_
#define _PROCEDURAL_H_

#include "Application.h"
#include "Camera.h"
#include "Utility.h"
#include "Vertex.h"

class ProceduralGeneration : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();

	void buildGrid(vec2 real_dims, glm::ivec2 dims);
	void buildPerlinTexture(glm::ivec2 dims, int octaves, float persistance);

	OpenGLData m_plane_mesh;
	unsigned int m_program_id;
	unsigned int m_perlin_texture;
	float* m_perlin_data;

	float m_scale;

	FlyCamera camera;

	TwBar* m_bar;
	bool m_draw_gizmos;
	float m_fps;

};

#endif // !_PROCEDURAL_H_
