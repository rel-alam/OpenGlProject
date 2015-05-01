#include "GameTreeSearch.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"

#define GSM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"

bool GameTreeSearch::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
	glClearColor(0.3f, 0.3f, 0.3f, 1);
	glEnable(GL_DEPTH_TEST);



	Gizmos::create();

	camera = FlyCamera();
	camera.setLookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	camera.setSpeed(40);
	camera.setPrespective(60, 1280 / 720, 0.1f, 1000.f);

	m_window = glfwGetCurrentContext();

	// create a new game of Connect four
	m_game = new ConnectFour();

	//create an AI just a random one
	m_ai = new MCTS(500);
	m_ai2 = new MCTS(500);

	return true;
}

void GameTreeSearch::shutdown()
{
	Gizmos::destroy();
	delete m_ai;
	delete m_game;
}
bool GameTreeSearch::update()
{
	if (Application::update() == false)
	{
		return false;
	}
	Gizmos::clear();


	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	camera.update(dt);

	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	vec4 green(0, 1, 0, 1);
	vec4 red(1, 0, 0, 1);
	vec4 blue(0, 0, 1, 1);
	vec4 yellow(1, 1, 0, 1);

	//for (int i = 0; i <= 20; ++i)
	//{
	//	Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10), i == 10 ? white : black);
	//	Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i), i == 10 ? white : black);
	//}


	static bool moveMade = false;

	if (m_game->getCurrentGameState() == Game::UNKNOWN)
	{
		if (m_game->getCurrentPlayer() == Game::PLAYER_ONE)
		{
			if (glfwGetMouseButton(m_window, 0) == GLFW_PRESS)
			{
				if (moveMade == false)
				{
					moveMade = true;
		
					double x = 0, y = 0;
					glfwGetCursorPos(m_window, &x, &y);
					m_pickPosition = camera.pickAgainstPlane((float)x, (float)y, vec4(0, 1, 0, 0));
					int column = (int)((m_pickPosition.z + ConnectFour::COLUMNS) / 2);
		
					if (m_game->isActionValid(column))
						m_game->performAction(column);
				}
			}
			else
				moveMade = false;
		}
		else
		{
			m_game->performAction(m_ai->makeDecision(*m_game));
		}
	}

	return true;
}

void GameTreeSearch::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_game->draw();

	Gizmos::draw(camera.getProjectionView());

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}