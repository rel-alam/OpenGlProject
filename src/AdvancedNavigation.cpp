#include "AdvancedNavigation.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"

#define GSM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "tiny_obj_loader.h"


void AdvancedNavigation::buildNavMesh()
{
	//Load our nav mesh from the obj file
	////////m_sponza_navmesh = LoadOBJ("./models/SponzaSimpleNavMesh.obj");

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;



	std::string err = tinyobj::LoadObj(shapes, materials, "./models/SponzaSimpleNavMesh.obj");
	if (err.size() != 0)
	{
		printf("%s\n", err.c_str());
		return;
	}

	if (shapes.size() != 1)
	{
		printf("OBJ did not have one mesh\n");
		return;
	}

	tinyobj::mesh_t* mesh = &shapes[0].mesh;

	//resize our nav_nodes vector to hold
	//the right number of nodes
	unsigned int index_count = mesh->indices.size();

	m_nav_nodes.resize(index_count / 3);

	//loop through all the indicies and build
	//our nodes - set the centres and corners

	for (unsigned int index = 0; index < index_count; index += 3)
	{
		unsigned int node_index = index / 3;

		for (unsigned int corner_index = 0; corner_index < 3; ++corner_index)
		{
			unsigned int vertex_index = mesh->indices[index + corner_index];
			m_nav_nodes[node_index].corners[corner_index].x = mesh->positions[vertex_index * 3 + 0];
			m_nav_nodes[node_index].corners[corner_index].y = mesh->positions[vertex_index * 3 + 1];
			m_nav_nodes[node_index].corners[corner_index].z = mesh->positions[vertex_index * 3 + 2];
		}

		m_nav_nodes[node_index].position = (m_nav_nodes[node_index].corners[0] + 
											m_nav_nodes[node_index].corners[1] + 
											m_nav_nodes[node_index].corners[2]) / 3.0f;
	}


	//loop through all our nodes
		//loop through all of them again
		//look for matching edges

	for (auto& first : m_nav_nodes)
	{
		unsigned int curr_edge = 0;

		first.edges[0] = 0;
		first.edges[1] = 0;
		first.edges[2] = 0;

		for (auto& second : m_nav_nodes)
		{
			if (&first == &second)
			{
				continue;
			}

			for (unsigned int EI = 0; EI < 3; ++EI)
			{
				if (first.corners[EI] == second.corners[0] && first.corners[(EI + 1) % 3] == second.corners[1] ||
					first.corners[EI] == second.corners[1] && first.corners[(EI + 1) % 3] == second.corners[2] ||
					first.corners[EI] == second.corners[2] && first.corners[(EI + 1) % 3] == second.corners[0] ||


					first.corners[EI] == second.corners[1] && first.corners[(EI + 1) % 3] == second.corners[0] ||
					first.corners[EI] == second.corners[2] && first.corners[(EI + 1) % 3] == second.corners[1] ||
					first.corners[EI] == second.corners[0] && first.corners[(EI + 1) % 3] == second.corners[2])
				{
					first.edges[curr_edge] = &second;
					first.edge_costs[curr_edge] = glm::distance(second.position, first.position);
					++curr_edge;
					break;
				}
			}

			if (curr_edge == 3)
			{
				break;
			}
		}
	}

}


bool AdvancedNavigation::startup()
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
	
	m_sponza_mesh = LoadOBJ("./models/SponzaSimple.obj");
	LoadShader("./shaders/navmesh_vertex.glsl", 0, "./shaders/navmesh_fragment.glsl", &m_program_id);
	buildNavMesh();

	return true;
}

void AdvancedNavigation::shutdown()
{
	Gizmos::destroy();
}
bool AdvancedNavigation::update()
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

	for (int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10), i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i), i == 10 ? white : black);
	}
	vec3 raise = vec3(0, 0.1f, 0);
	for (unsigned int node_index = 0; node_index < m_nav_nodes.size(); ++node_index)
	{

		Gizmos::addAABBFilled(m_nav_nodes[node_index].corners[0] + raise, vec3(0.1f), vec4(1, 0, 0, 1));
		Gizmos::addAABBFilled(m_nav_nodes[node_index].corners[1] + raise, vec3(0.1f), vec4(1, 0, 0, 1));
		Gizmos::addAABBFilled(m_nav_nodes[node_index].corners[2] + raise, vec3(0.1f), vec4(1, 0, 0, 1));

		Gizmos::addAABBFilled(m_nav_nodes[node_index].position + raise, vec3(0.1f), vec4(0, 0, 1, 1));

		Gizmos::addLine(m_nav_nodes[node_index].corners[0] + raise, m_nav_nodes[node_index].corners[1] + raise, vec4(0, 1, 0, 1));
		Gizmos::addLine(m_nav_nodes[node_index].corners[1] + raise, m_nav_nodes[node_index].corners[2] + raise, vec4(0, 1, 0, 1));
		Gizmos::addLine(m_nav_nodes[node_index].corners[2] + raise, m_nav_nodes[node_index].corners[0] + raise, vec4(0, 1, 0, 1));

		for (unsigned int edge = 0; edge < 3; ++edge)
		{
			if (m_nav_nodes[node_index].edges[edge] != 0)
			{
				Gizmos::addLine(m_nav_nodes[node_index].position + raise, m_nav_nodes[node_index].edges[edge]->position + raise, vec4(1, 1, 0, 1));
			}
			
		}
	}

	return true;
}

void AdvancedNavigation::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(camera.getProjectionView());

	glUseProgram(m_program_id);

	int proj_view_uniform = glGetUniformLocation(m_program_id, "proj_view");

	glUniformMatrix4fv(proj_view_uniform, 1, GL_FALSE, (float*)&camera.getProjectionView());

	glBindVertexArray(m_sponza_mesh.m_VAO);

	glDrawElements(GL_TRIANGLES, m_sponza_mesh.m_index_count, GL_UNSIGNED_INT, 0);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}