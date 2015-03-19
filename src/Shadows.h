#ifndef _SHADOWS_H_
#define _SHADOWS_H_

#include "Application.h"
#include "Camera.h"

#include "Vertex.h"
class Shadows : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();

	virtual bool update();
	virtual void draw();

	void buildMeshes();
	void buildShadowMap();

	FlyCamera camera;

	OpenGLData m_bunny;
	OpenGLData m_plane;

	unsigned int m_fbo;
	unsigned int m_fbo_depth;

	unsigned int m_shadowmap_program;
	unsigned int m_diffuse_shadowed_program;

	vec3 m_light_dir;
	mat4 m_light_matrix;

};

#endif // !CAM_PROJ_H_
