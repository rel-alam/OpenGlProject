#ifndef _PARTICLES_H_
#define _PARTICLES_H_

#include "Application.h"
#include "Camera.h"
#include "Emitter.h"

class Particles : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();

	virtual bool update();
	virtual void draw();

	FlyCamera camera;

	Emitter m_emitter;
	unsigned int m_program_id;

};

#endif // !CAM_PROJ_H_
