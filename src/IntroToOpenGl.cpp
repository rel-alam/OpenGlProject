#include "IntroToOpenGl.h"




bool IntroToOpenGl::startup()
{

	if (Application::startup() == false)
	{
		return false;
	}
	glClearColor(0.3f, 0.3f, 0.3f, 1);
	glEnable(GL_DEPTH_TEST);




	Gizmos::create();
	m_projection = glm::perspective(glm::radians(60.f), 1280.f / 720.f, 0.1f, 1000.f);

	m_view = glm::lookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	m_camera_x = -10.0f;
	m_camera_z = -10.0f;

	m_timer = 0.0f;

	return true;
}

void IntroToOpenGl::shutdown()
{
	Gizmos::destroy();
}

bool IntroToOpenGl::update()
{
	if (Application::update() == false)
	{
		return false;
	}
	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	

	mat4 view = glm::lookAt(vec3(m_camera_x, 10, m_camera_z), vec3(0, 0, 0), vec3(0, 1, 0));
	m_timer += dt;
	//camera_x = sinf(timer) * 10;
	//camera_z = cosf(timer) * 10;

	

	Gizmos::clear();


	Gizmos::addTransform(mat4(1));

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

	//Gizmos::addTri(vec3(0, 1, 0), vec3(2, -1, 1), vec3(-3, -2, 4), green);
	//
	//Gizmos::addAABB(vec3(2.5f, 5, 0), vec3(1, 1, 1), blue);
	//Gizmos::addAABBFilled(vec3(-2.5f, 5, 0), vec3(1, 1, 1), blue);

	mat4 sun_transform = BuildOrbitMatrix(m_timer, 0, 0);

	//for (int planet_index = 0; planet_index < 15; ++planet_index)
	//{
	//	mat4 planet_mat = sun_transform * BuildOrbitMatrix(m_timer / (float)planet_index, (5 + planet_index) / 2.0f, m_timer / (float)planet_index);
	//	Gizmos::addSphere(planet_mat[3].xyz, 0.5f, 20, 20, yellow, &planet_mat);
	//}



	mat4 planet_1 = sun_transform * BuildOrbitMatrix(m_timer * 5, 5, m_timer * 0.5f);
	mat4 planet_2 = sun_transform * BuildOrbitMatrix(m_timer * 3, 3.5f, m_timer * 1.5f);
	mat4 planet_3 = sun_transform * BuildOrbitMatrix(m_timer * 2, 6, m_timer * 0.25f);
	mat4 planet_4 = sun_transform * BuildOrbitMatrix(m_timer * 15, 2, m_timer * 5.f);


	Gizmos::addSphere(sun_transform[3].xyz, 1.0f, 20, 20, yellow, &sun_transform);

	Gizmos::addSphere(planet_1[3].xyz, 1.0f, 20, 20, red, &planet_1);
	Gizmos::addSphere(planet_2[3].xyz, 0.25f, 20, 20, green, &planet_2);
	Gizmos::addSphere(planet_3[3].xyz, 0.5f, 20, 20, black, &planet_3);
	Gizmos::addSphere(planet_4[3].xyz, 0.90f, 20, 20, blue, &planet_4);


	Gizmos::draw(m_projection, view);

	
	return true;
}


void IntroToOpenGl::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

mat4 IntroToOpenGl::BuildOrbitMatrix(float local_roatation, float radius, float orbit_rotation)
{
	mat4 result =
		glm::rotate(orbit_rotation, vec3(0, 1, 0))  *
		glm::translate(vec3(radius, 0, 0))  * glm::rotate(local_roatation, vec3(0, 1, 0));
	return result;
}