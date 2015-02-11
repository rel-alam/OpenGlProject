#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Gizmos.h"
#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

class Camera
{
public:
	Camera();
	~Camera();

	virtual bool update(float a_deltaTime) = 0;
	void setPrespective(float fov, float aspect, float nears, float fars);
	void setLookAt(vec3 from, vec3 to, vec3 up);
	void setPosition(vec3 Position);
	mat4 getWorldTransform();
	mat4 getView();
	mat4 getProjection();
	mat4 getProjectionView();

	void updateProjectionViewTransform();


	mat4 m_world;
	mat4 m_view;
	mat4 m_proj;
	mat4 m_projView;
	float m_sensitivity;
};

class FlyCamera : public Camera
{
public:
	bool update(float a_deltaTime);
	void setSpeed(float a_speed);

private:
	float m_speed;
	
	vec3 m_up;
};

#endif // !_CAMERA_H_
