#include "Animation.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"

#define GSM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "Utility.h"
bool Animation::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
	glClearColor(0.3f, 0.3f, 0.3f, 1);
	glEnable(GL_DEPTH_TEST);

	m_timer = 0; 

	Gizmos::create();

	camera = FlyCamera();
	camera.setLookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	camera.setSpeed(1);
	camera.setPrespective(60, 1280 / 720, 0.1f, 1000.f);

	m_file = new FBXFile();
	m_file->load("./models/characters/Pyro/pyro.fbx");
	m_file->initialiseOpenGLTextures();

	GenerateGLMeshes(m_file);

	LoadShader("./shaders/skinned_vertex.glsl", "./shaders/skinned_fragment.glsl", &m_program_id);



	return true;
}

void Animation::shutdown()
{
	m_file->unload();
	delete m_file;

	Gizmos::destroy();
}
bool Animation::update()
{
	if (Application::update() == false)
	{
		return false;
	}


	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);
	m_timer += dt;

	Gizmos::clear();
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

	FBXSkeleton* skele = m_file->getSkeletonByIndex(0);
	FBXAnimation* anim = m_file->getAnimationByIndex(0);

	skele->evaluate(anim, m_timer);

	for (unsigned int i = 0; i < skele->m_boneCount; ++i)
	{
		skele->m_nodes[i]->updateGlobalTransform();
		mat4 node_global = skele->m_nodes[i]->m_globalTransform;
		vec3 node_pos = node_global[3].xyz;

		Gizmos::addAABBFilled(node_pos, vec3(2.f), vec4(1, 0, 0, 1), &node_global);
		if (skele->m_nodes[i]->m_parent != nullptr)
		{
			vec3 parent_pos = skele->m_nodes[i]->m_parent->m_globalTransform[3].xyz;
			Gizmos::addLine(node_pos, parent_pos, vec4(0, 1, 0, 1));
		}
	}


	return true;
}

void Animation::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(camera.getProjectionView());
	glUseProgram(m_program_id);
	
	int view_proj_uniform = glGetUniformLocation(m_program_id, "projection_view");

	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE, (float*)&camera.getProjectionView());
	int diffuse_location = glGetUniformLocation(m_program_id, "diffuse");
	glUniform1i(diffuse_location, 0);

	for (unsigned int i = 0; i < m_meshes.size(); ++i)
	{
		FBXMeshNode* curr_mesh = m_file->getMeshByIndex(i);

		mat4 world_transform = curr_mesh->m_globalTransform;
		FBXMaterial* mesh_material = curr_mesh->m_material;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh_material->textures[FBXMaterial::DiffuseTexture]->handle);


		int world_uniform = glGetUniformLocation(m_program_id, "world");
		glUniformMatrix4fv(world_uniform, 1, GL_FALSE, (float*)&world_transform);

		glBindVertexArray(m_meshes[i].m_VAO);
		glDrawElements(GL_TRIANGLES, m_meshes[i].m_index_count, GL_UNSIGNED_INT, 0);
	}

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}


void Animation::GenerateGLMeshes(FBXFile* fbx)
{
	unsigned int mesh_count = fbx->getMeshCount();
	m_meshes.resize(mesh_count);

	for (unsigned int mesh_index = 0; mesh_index < mesh_count; ++mesh_index)
	{
		FBXMeshNode* curr_mesh = fbx->getMeshByIndex(mesh_index);

		m_meshes[mesh_index].m_index_count = curr_mesh->m_indices.size();
		glGenBuffers(1, &m_meshes[mesh_index].m_VBO);
		glGenBuffers(1, &m_meshes[mesh_index].m_IBO);
		glGenVertexArrays(1, &m_meshes[mesh_index].m_VAO);

		glBindVertexArray(m_meshes[mesh_index].m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_meshes[mesh_index].m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(FBXVertex)* curr_mesh->m_vertices.size(), curr_mesh->m_vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshes[mesh_index].m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* curr_mesh->m_indices.size(), curr_mesh->m_indices.data(), GL_STATIC_DRAW);


		glEnableVertexAttribArray(0); //pos
		glEnableVertexAttribArray(1); //tex coords

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::PositionOffset);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::TexCoord1Offset);


		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	}



}