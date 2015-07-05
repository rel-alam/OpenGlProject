#include "gl_core_4_4.h"

#include <GLFW\glfw3.h>
#include <cstdio>
#include "Gizmos.h"
#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include "ProceduralGeneration.h"
#include "Animation.h"
#include "GPUParticles.h"
#include "PhysicallyBasedRendering.h"
#include "RenderTargets.h"


int main()
{
	//OpenGlStuff
	//RenderTargets app;
	//PhysicalBasedRendering app;
	//GPUParticles app;
	//Animation app;
	ProceduralGeneration app;

	if (app.startup() == false)
	{
		return -1;
	}

	while (app.update() == true)
	{
		app.draw();
	}

	app.shutdown();

	return 0;
}