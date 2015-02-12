#ifndef _LIGHTING_H_
#define _LIGHTING_H_

#include "Application.h"
#include "Camera.h"
#include "tiny_obj_loader.h"


struct OpenGLData
{
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

	unsigned int m_index_count;
};

class Lighting : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();

	virtual bool update();
	virtual void draw();

	void createOpenGLBuffers(std::vector<tinyobj::shape_t> &shapes);
	void cleanUpOpenGLBuffers();

	void reloadShader();

	FlyCamera camera;

	unsigned int m_program_id;
	std::vector<OpenGLData> m_gl_data;

};

#endif