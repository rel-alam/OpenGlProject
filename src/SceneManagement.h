#ifndef SCENE_MANAGEMENT_H
#define SCENE_MANAGEMENT_H

#include "Application.h"
#include "Camera.h"

#include <vector>

struct AABB
{
	vec3 min, max;
};

struct MeshObject
{
	unsigned int m_vbo;
	unsigned int m_vao;
	unsigned int m_ibo;
	unsigned int m_index_count;
	mat4 m_transform;
	AABB m_aabb;
};
class SceneManagement : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();

	virtual bool update();
	virtual void draw();

	FlyCamera camera;
	std::vector<MeshObject> m_meshes;

	vec3 m_ambient_light;
	vec3 m_light_dir;
	vec3 m_light_color;
	vec3 m_material_color;
	float m_specular_power;

	unsigned int m_program_id;

};

#endif // !SCENE_MANAGEMENT_H
