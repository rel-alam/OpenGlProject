#ifndef CAM_PROJ_H_
#define CAM_PROJ_H_

#include "Application.h"
#include "Camera.h"
#include "AIBase.h"
#include "ConnectFour.h"

class GameTreeSearch : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();

	virtual bool update();
	virtual void draw();

	FlyCamera camera;

	Game* m_game;
	AI* m_ai;
	AI* m_ai2;

	GLFWwindow* m_window;

	vec3 m_pickPosition;

};

#endif // !CAM_PROJ_H_
