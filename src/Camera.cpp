#include "Camera.h"
#include <GLFW\glfw3.h>

Camera::Camera()
{

}

Camera::~Camera()
{

}
void Camera::setPrespective(float fov, float aspect, float near, float far)
{
	m_proj = glm::perspective(glm::radians(fov), aspect, near, far);
}

void Camera::setLookAt(vec3 from, vec3 to, vec3 up)
{
	m_view = glm::lookAt(from, to, up);
}

void Camera::setPosition(vec3 Position)
{
//	m_world[3] = Position;
}


mat4 Camera::getWorldTransform()
{
	return m_world;
}

mat4 Camera::getView()
{
	return m_view;
}

mat4 Camera::getProjection()
{
	return m_proj;
}
mat4 Camera::getProjectionView()
{
	return m_projView;
}

void Camera::updateProjectionViewTransform()
{
	m_view = glm::inverse(m_world);
	m_projView = m_view * m_proj;
}

bool FlyCamera::update(float a_deltaTime)
{
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W) == GLFW_PRESS)
	{
		m_world[3] -= m_world[2] * m_speed * a_deltaTime;
	}

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_A) == GLFW_PRESS)
	{
		m_world[3] -= m_world[0] * m_speed * a_deltaTime;
	}

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S) == GLFW_PRESS)
	{
		m_world[3] += m_world[2] * m_speed * a_deltaTime;
	}

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_D) == GLFW_PRESS)
	{
		m_world[3] += m_world[0] * m_speed * a_deltaTime;
	}

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_E) == GLFW_PRESS)
	{
		m_world[3] += m_world[1] * m_speed * a_deltaTime;
	}
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_Q) == GLFW_PRESS)
	{
		m_world[3] -= m_world[1] * m_speed * a_deltaTime;
	}

	GLFWwindow* window = glfwGetCurrentContext();
	double x_delta, y_delta;
	glfwGetCursorPos(window, &x_delta, &y_delta);
	glfwSetCursorPos(window, 1200.f / 2.f, 720.f / 2.f);
	
	x_delta -= (1280.f / 2.f);
	y_delta -= (720.f / 2.f);

	x_delta /= (1280.f / 2.f);
	y_delta /= (720.f / 2.f);

	if (glfwGetMouseButton(window, 1))
	{

		vec3 camera_right = (vec3)m_world[0];

		mat4 yaw = glm::rotate((float)x_delta, vec3(0, 1, 0));
		mat4 pitch = glm::rotate((float)y_delta, camera_right);
		mat4 rot = yaw * pitch;

		m_world[0] = rot * m_world[0];
		m_world[1] = rot * m_world[1];
		m_world[2] = rot * m_world[2];

	}

	m_view = glm::inverse(m_world);

	updateProjectionViewTransform();
	return true;
}

void FlyCamera::setSpeed(float a_speed)
{
	m_speed = a_speed;
}