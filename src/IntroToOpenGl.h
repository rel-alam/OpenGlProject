#ifndef __INTRO_OGL_H__
#define __INTRO_OGL_H__

#include "Application.h"


using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

class IntroToOpenGl : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();

	virtual bool update();
	virtual void draw();

	mat4 m_projection;
	mat4 m_view;
	
	float m_camera_x;
	float m_camera_z;

	float m_timer;

	mat4 BuildOrbitMatrix(float local_roatation, float radius, float orbit_rotation);
};

#endif