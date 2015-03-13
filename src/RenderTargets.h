#ifndef _REND_TARG_H_
#define _REND_TARG_H_

#include "Application.h"
#include "Camera.h"
#include "Vertex.h"
class RenderTargets : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();

	virtual bool update();
	virtual void draw();

	void generateFrameBuffer();
	void generatePlane();

	FlyCamera camera;

	unsigned int m_fbo;
	unsigned int m_fbo_texture;
	unsigned int m_fbo_depth;

	OpenGLData m_plane;

	unsigned int m_program_id;

};

#endif // !_REND_TARG_H_
