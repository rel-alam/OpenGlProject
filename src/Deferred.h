#ifndef _DEFERRED_H_
#define _DEFERRED_H_

#include "Application.h"
#include "Camera.h"
#include "Vertex.h"
class Deferred : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();

	virtual bool update();
	virtual void draw();

	void renderDirectionalLight(vec3 light_dir, vec3 light_color);
	void renderPointLight(vec3 position, float radius, vec3 light_color);

	void buildMesh();
	void buildQuad();
	void buildCube();
	void buildGBuffer();
	void buildLightBuffer();

	// load up a mesh
	OpenGLData m_light_cube;
	OpenGLData m_bunny;
	OpenGLData m_screenspace_quad;

	//generate a g buffer
	unsigned int m_gbuffer_fbo;
	unsigned int m_albedo_texture;
	unsigned int m_position_texture;
	unsigned int m_normals_texture;
	unsigned int m_gbuffer_depth;

	//render lights
	unsigned int m_light_fbo;
	unsigned int m_light_texture;


	//we need our shaders
	unsigned int m_gbuffer_program;
	unsigned int m_directional_light_program;
	unsigned int m_point_light_program;
	unsigned int m_spot_light_program;
	unsigned int m_composite_program;

	FlyCamera camera;

};

#endif // !CAM_PROJ_H_
