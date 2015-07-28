#ifndef ASSIGNMENT_H_
#define ASSIGNMENT_H_

#include "Application.h"
#include "Camera.h"
#include "Utility.h"
#include "Vertex.h"
#include "GPUEmitter.h"
#include "FBXFile.h"
#include "Deferred.h"
#include "ProceduralGeneration.h"

class Assignment : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();

	virtual bool update();
	virtual void draw();

#pragma region Animation call functions
	void GenerateGLMeshes(FBXFile* fbx);
	void EvaluateSkeleton(FBXAnimation* anim, FBXSkeleton* skeleton, float timer);
	void UpdateBones(FBXSkeleton* skeleton);
	float m_timer;
	unsigned int m_animation_program_id;
	FBXFile* m_file;
	std::vector<OpenGLData> m_meshes;
#pragma endregion




	OpenGLData m_plane_mesh;
	unsigned int m_program_id;
	unsigned int m_perlin_texture;
	float* m_perlin_data;

	float m_scale;

	
	bool m_draw_gizmos;
	float m_fps;
	GPUPointEmitter m_emitter;
	float m_time;
	FlyCamera camera;

	void reloadShaders();

	ProceduralGeneration proceduralGen;


	//Deferred Stuff
	unsigned int m_gbuffer_program;
	unsigned int m_directional_light_program;
	unsigned int m_point_light_program;
	unsigned int m_spot_light_program;
	unsigned int m_composite_program;

	void renderDirectionalLight(vec3 light_dir, vec3 light_color, FlyCamera camera);
	void renderPointLight(vec3 position, float radius, vec3 light_color, FlyCamera camera);

	void buildMesh();
	void buildQuad();
	void buildCube();
	void buildGBuffer();
	void buildLightBuffer();

	OpenGLData m_light_cube;
	OpenGLData m_bunny;
	OpenGLData m_screenspace_quad;

	//generate a g buffer
	unsigned int m_gbuffer_fbo;
	unsigned int m_albedo_texture;
	unsigned int m_position_texture;
	unsigned int m_normals_texture;
	unsigned int m_gbuffer_depth;

	//render lights
	unsigned int m_light_fbo;
	unsigned int m_light_texture;


	vec3 lightpos = vec3(1, 8, 0);
	vec3 lightcolour = vec3(1, 1, 1);

	vec3 particle_pos = vec3(0, 0, 0);

	TwBar* m_bar;
	TwBar* m_light_bar;




};

#endif // !ASSIGNMENT_H_
