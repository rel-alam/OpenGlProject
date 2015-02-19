#include "Quaternions.h"
#include "glm_header.h"
#include "Vertex.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Utility.h"
#include "Vertex.h"
#include "stb_image.h"


bool Quaternions::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1);
	glEnable(GL_DEPTH_TEST);


	Gizmos::create();

	glm::quat boring_quaternion(1, 0, 0, 0);
	glm::quat euler_quat(vec3(3, 5, 7));

	glm::quat mixed_quat = glm::slerp(boring_quaternion, euler_quat, 0.8f);


	m_camera = new FlyCamera();
	m_camera->setLookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	m_camera->setSpeed(1);
	m_camera->setPrespective(60, 1280 / 720, 0.1f, 1000.f);

	m_hip_frames[0].position = vec3(0, 5, 0);
	m_hip_frames[0].rotation = glm::quat(vec3(-1, 0, 0));

	m_knee_frames[0].position = vec3(0, -2.5, 0);
	m_knee_frames[0].rotation = glm::quat(vec3(-1, 0, 0));

	m_ankle_frames[0].position = vec3(0, -2.5, 0);
	m_ankle_frames[0].rotation = glm::quat(vec3(-1, 0, 0));




	m_hip_frames[1].position = vec3(0, 5, 0);
	m_hip_frames[1].rotation = glm::quat(vec3(1, 0, 0));

	m_knee_frames[1].position = vec3(0, -2.5, 0);
	m_knee_frames[1].rotation = glm::quat(vec3(0, 0, 0));

	m_ankle_frames[1].position = vec3(0, -2.5, 0);
	m_ankle_frames[1].rotation = glm::quat(vec3(0, 0, 0));


	return true;
}


mat4 EvaluateKeyFrames(KeyFrame start, KeyFrame end, float t)
{
	vec3 pos = glm::mix(start.position, end.position, t);
	glm::quat rot = glm::slerp(start.rotation, end.rotation, t);

	mat4 result = glm::translate(pos) * glm::toMat4(rot);

	return result;
}






void Quaternions::shutdown()
{
	Application::shutdown();
}
bool Quaternions::update()
{
	if (Application::update() == false)
	{
		return false;
	}
	Gizmos::clear();


	float dt = (float)glfwGetTime();
	glfwSetTime(0.0f);

	m_timer += dt * 2;

	float sin_wave = sinf(m_timer) * 0.5f + 0.5f;

	m_hip_bone = EvaluateKeyFrames(m_hip_frames[0], m_hip_frames[1], sin_wave);
	m_knee_bone = EvaluateKeyFrames(m_knee_frames[0], m_knee_frames[1], sin_wave);
	m_ankle_bone = EvaluateKeyFrames(m_ankle_frames[0], m_ankle_frames[1], sin_wave);

	mat4 global_hip = m_hip_bone;
	mat4 global_knee = m_hip_bone * m_knee_bone;
	mat4 global_ankle = global_knee * m_ankle_bone;

	vec3 hip_pos = global_hip[3].xyz;
	vec3 knee_pos = global_knee[3].xyz;
	vec3 ankle_pos = global_ankle[3].xyz;

	Gizmos::addAABBFilled(hip_pos, vec3(0.5f), vec4(1, 1, 0, 1), &global_hip);
	Gizmos::addAABBFilled(knee_pos, vec3(0.5f), vec4(1, 0, 1, 1), &global_knee);
	Gizmos::addAABBFilled(ankle_pos, vec3(0.5f), vec4(0, 1, 1, 1), &global_ankle);

	Gizmos::addLine(hip_pos, knee_pos, vec4(0, 1, 0, 1), vec4(1, 0, 0, 1));
	Gizmos::addLine(knee_pos, ankle_pos, vec4(0, 1, 0, 1), vec4(1, 0, 0, 1));

	

	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	vec4 green(0, 1, 0, 1);
	vec4 red(1, 0, 0, 1);
	vec4 blue(0, 0, 1, 1);
	vec4 yellow(1, 1, 0, 1);

	for (int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10), i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i), i == 10 ? white : black);
	}

	m_camera->update(dt);

	return true;
}
void Quaternions::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	Gizmos::draw(m_camera->getProjectionView());
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}
