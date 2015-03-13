#ifndef _POST_PROCESS_H
#define _POST_PROCESS_H

#include "Application.h"
#include "Camera.h"
#include "Vertex.h"
class PostProcess : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();

	virtual bool update();
	virtual void draw();

	FlyCamera camera;


	void generateFrameBuffer();
	void generateScreenspaceQuad();

	unsigned int m_fbo;
	unsigned int m_fbo_texture;
	unsigned int m_fbo_depth;

	unsigned int m_program_id;

	OpenGLData m_quad;

};

#endif // !_POST_PROCESS_H
