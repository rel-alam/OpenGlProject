#include "Camera.h"
#include <GLFW\glfw3.h>

Camera::Camera()
{
	m_sensitivity = 5;
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
	m_world = glm::inverse(m_view);
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
	return m_proj * m_view;
}

void Camera::updateProjectionViewTransform()
{
	m_view = glm::inverse(m_world);
	m_projView = m_proj * m_view;
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

	if (glfwGetMouseButton(window, 1))
	{
		double x_delta, y_delta;
		glfwGetCursorPos(window, &x_delta, &y_delta);
		glfwSetCursorPos(window, 1280.f / 2.f, 720.f / 2.f);

		x_delta -= (1280.f / 2.f);
		y_delta -= (720.f / 2.f);

		x_delta /= (1280.f / 2.f);
		y_delta /= (720.f / 2.f);

		x_delta = x_delta * -m_sensitivity;
		y_delta = y_delta * -m_sensitivity;
		
	

		vec3 camera_right = (vec3)m_world[0];

		mat4 yaw = glm::rotate((float)x_delta, vec3(0, 1, 0));
		mat4 pitch = glm::rotate((float)y_delta, camera_right);
		mat4 rot = yaw * pitch;

		m_world[0] = rot * m_world[0];
		m_world[1] = rot * m_world[1];
		m_world[2] = rot * m_world[2];

	}

	m_world[3][3] = 1;
	m_proj = glm::perspective(glm::radians(60.f), 1280.f/720.f, 0.1f, 1000.f);
	m_view = glm::inverse(m_world);

	updateProjectionViewTransform();
	return true;
}

void FlyCamera::setSpeed(float a_speed)
{
	m_speed = a_speed;
}

vec3 Camera::pickAgainstPlane(float x, float y, vec4 plane)
{
	float nxPos = x / 1280.0;
	float nyPos = y / 720.0;

	float sxPos = nxPos - 0.5;
	float syPos = nyPos - 0.5;

	float fxPos = sxPos * 2;
	float fyPos = syPos * -2;

	mat4 inv_viewproj = glm::inverse(m_projView);

	vec4 mouse_pos(fxPos, fyPos, 1, 1);
	vec4 world_pos = inv_viewproj * mouse_pos;

	world_pos /= world_pos.w;

	vec3 cam_pos = m_world[3].xyz();
	vec3 dir = world_pos.xyz() - cam_pos;

	float t = -(glm::dot(cam_pos, plane.xyz()) + plane.w) / (glm::dot(dir, plane.xyz()));

	vec3 result = cam_pos + dir * t;

	return result;
}