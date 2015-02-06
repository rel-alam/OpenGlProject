#ifndef CAM_PROJ_H_
#define CAM_PROJ_H_

#include "Application.h"
#include "Camera.h"
class CameraAndProjection : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();

	virtual bool update();
	virtual void draw();

	FlyCamera* camera;

};

#endif // !CAM_PROJ_H_
