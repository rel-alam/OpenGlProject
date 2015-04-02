#ifndef _PBR_H_
#define _PBR_H_

#include "Application.h"
#include "Camera.h"
#include "tiny_obj_loader.h"
#include "Utility.h"


//struct OpenGLData
//{
//	unsigned int m_VAO;
//	unsigned int m_VBO;
//	unsigned int m_IBO;
//
//	unsigned int m_index_count;
//};

class PhysicalBasedRendering : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();

	virtual bool update();
	virtual void draw();

	void createOpenGLBuffers(std::vector<tinyobj::shape_t> &shapes);
	void cleanUpOpenGLBuffers();

	void reloadShader();

	FlyCamera m_camera;

	unsigned int m_program_id;
	std::vector<OpenGLData> m_gl_data;

	vec3 m_ambient_light;
	vec3 m_light_dir;
	vec3 m_light_color;
	vec3 m_material_color;

	float m_specular_power;

};

#endif