#ifndef _ADANCED_NAVIGATION_H_
#define _ADANCED_NAVIGATION_H_

#include "Application.h"
#include "Camera.h"
#include <vector>
#include "Utility.h"

struct NavMeshNode
{
	//centre point of the polygon
	vec3 position;
	vec3 corners[3];
	NavMeshNode* edges[3];

	//A* data
	unsigned int flags;
	float edge_costs[5];
};

class AdvancedNavigation : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();

	void buildNavMesh();

	//what are we going to need
	unsigned int m_program_id;
	OpenGLData m_sponza_mesh;
	std::vector<NavMeshNode> m_nav_nodes;


	FlyCamera camera;
};

#endif // !_ADANCED_NAVIGATION_H_
