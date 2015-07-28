#include "Assignment.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"

#include "tiny_obj_loader.h"

#define GSM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"


void Assignment::reloadShaders()
{

	LoadShader("./shaders/skinned_vertex.glsl", 0, "./shaders/skinned_fragment.glsl", &m_animation_program_id);
	LoadShader("./shaders/perlin_vertex.glsl", 0, "./shaders/perlin_fragment.glsl", &proceduralGen.m_program_id);
	LoadShader("./shaders/point_light_vertex.glsl", 0, "./shaders/point_light_fragment.glsl", &m_point_light_program);
	LoadShader("./shaders/gBuffer_vertex.glsl", 0, "./shaders/gBuffer_fragment.glsl", &m_gbuffer_program);
	LoadShader("./shaders/composite_vertex.glsl", 0, "./shaders/composite_fragment.glsl", &m_composite_program);
	LoadShader("./shaders/composite_vertex.glsl", 0, "./shaders/directional_light_fragment.glsl", &m_directional_light_program);

}

bool Assignment::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
	m_draw_gizmos = true;
	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(1280, 720);

	glClearColor(0.3f, 0.3f, 0.3f, 1);
	glEnable(GL_DEPTH_TEST);

	glfwSetMouseButtonCallback(m_window, OnMouseButton);
	glfwSetCursorPosCallback(m_window, OnMousePosition);
	glfwSetScrollCallback(m_window, OnMouseScroll);
	glfwSetKeyCallback(m_window, OnKey);
	glfwSetCharCallback(m_window, OnChar);
	glfwSetWindowSizeCallback(m_window, OnWindowResize);

	Gizmos::create();

	camera = FlyCamera();
	camera.setLookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	camera.setSpeed(40);
	camera.setPrespective(60, 1280 / 720, 0.1f, 1000.f);



	m_bar = TwNewBar("Control Panel");
	m_light_bar = TwNewBar("Light Panel");

	
	TwAddVarRW(m_bar, "Draw Gizmos", TW_TYPE_BOOL8, &m_draw_gizmos, "");
	TwAddVarRO(m_bar, "FPS", TW_TYPE_FLOAT, &m_fps, "");
	TwAddSeparator(m_bar, NULL, NULL);

	TwAddVarRW(m_bar, "Set Scale", TW_TYPE_FLOAT, &proceduralGen.m_scale, "min=1 max=1000");
	TwAddVarRW(m_bar, "Real Dims Width", TW_TYPE_FLOAT, &proceduralGen.a_realDims.x, "min=1 max=1000");
	TwAddVarRW(m_bar, "Real Dims Height", TW_TYPE_FLOAT, &proceduralGen.a_realDims.y, "min=1 max=1000");
	TwAddVarRW(m_bar, "Dims Width", TW_TYPE_INT32, &proceduralGen.a_dims.x, "min=1 max=1000");
	TwAddVarRW(m_bar, "Dims Height", TW_TYPE_INT32, &proceduralGen.a_dims.y, "min=1 max=1000");

	TwAddVarRW(m_light_bar, "PosX", TW_TYPE_FLOAT, &lightpos.x, "group='Light Pos'");
	TwAddVarRW(m_light_bar, "PosY", TW_TYPE_FLOAT, &lightpos.y, "group='Light Pos'");
	TwAddVarRW(m_light_bar, "PosZ", TW_TYPE_FLOAT, &lightpos.z, "group='Light Pos'");
	TwAddVarRW(m_light_bar, "Set Light 1 Colour", TW_TYPE_COLOR3F, &lightcolour, "");

	TwAddSeparator(m_bar, NULL, NULL);
	TwAddVarRW(m_bar, "Set Particle Pos", TW_TYPE_DIR3F, &particle_pos, "");


	proceduralGen.startup();
	
	m_emitter.Init(100000, particle_pos, 0.5, 2, 5, 0.8f, 1.0f, 10, 0.01f, vec4(1, 0, 0, 1), vec4(0, 0.4f, 0, 1));
	m_time = 0;

	//Create the fbx file and load it in
	m_file = new FBXFile();
	m_file->load("./models/characters/Pyro/pyro.fbx");
	m_file->initialiseOpenGLTextures();
	GenerateGLMeshes(m_file);
	LoadShader("./shaders/skinned_vertex.glsl", 0, "./shaders/skinned_fragment.glsl", &m_animation_program_id);


	buildMesh();
	buildQuad();
	buildCube();
	buildGBuffer();
	buildLightBuffer();

	LoadShader("./shaders/gBuffer_vertex.glsl", 0, "./shaders/gBuffer_fragment.glsl", &m_gbuffer_program);
	LoadShader("./shaders/composite_vertex.glsl", 0, "./shaders/composite_fragment.glsl", &m_composite_program);
	LoadShader("./shaders/composite_vertex.glsl", 0, "./shaders/directional_light_fragment.glsl", &m_directional_light_program);
	LoadShader("./shaders/point_light_vertex.glsl", 0, "./shaders/point_light_fragment.glsl", &m_point_light_program);


	return true;
}

void Assignment::shutdown()
{
	m_file->unload();
	delete m_file;

	Gizmos::destroy();
}

bool Assignment::update()
{
	if (Application::update() == false)
	{
		return false;
	}
	Gizmos::clear();

	if (glfwGetKey(m_window, GLFW_KEY_R))
	{
		reloadShaders();
	}
	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	m_fps = 1 / dt;
	m_time += dt;

	if (glfwGetKey(m_window, GLFW_KEY_Z))
	{
		m_scale += 5 * dt;
	}
	if (glfwGetKey(m_window, GLFW_KEY_X))
	{
		m_scale -= 5 * dt;
	}

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

	//Getting index of skeleton and animation
	FBXSkeleton* skele = m_file->getSkeletonByIndex(0);
	FBXAnimation* anim = m_file->getAnimationByIndex(0);

	EvaluateSkeleton(anim, skele, m_time);

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
	proceduralGen.update();
	return true;
}

void Assignment::draw()
{
	////////////////////////////////
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, m_gbuffer_fbo);

	glClearColor(0, 0, 0, 0);

	glClear(GL_DEPTH_BUFFER_BIT);

	vec4 clear_color = vec4(0, 0, 0, 0);
	vec4 clear_normal = vec4(0.5f, 0.5f, 0.5f, 0.5f);

	glClearBufferfv(GL_COLOR, 0, (float*)&clear_color);
	glClearBufferfv(GL_COLOR, 1, (float*)&clear_color);
	glClearBufferfv(GL_COLOR, 2, (float*)&clear_normal);
	

	glUseProgram(m_gbuffer_program);

	int view_uniform = glGetUniformLocation(m_gbuffer_program, "view");
	int view_proj_uniform = glGetUniformLocation(m_gbuffer_program, "view_proj");

	glUniformMatrix4fv(view_uniform, 1, GL_FALSE, (float*)&camera.m_view);
	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE, (float*)&camera.m_projView);
	glBindVertexArray(m_bunny.m_VAO);
	glDrawElements(GL_TRIANGLES, m_bunny.m_index_count, GL_UNSIGNED_INT, 0);

	//////////////////////////////
	proceduralGen.draw(camera);

#pragma region Animation
	glUseProgram(m_animation_program_id);
	int view_anim_proj_uniform = glGetUniformLocation(m_animation_program_id, "projection_view");
	glUniformMatrix4fv(view_anim_proj_uniform, 1, GL_FALSE, (float*)&camera.getProjectionView());

	view_uniform = glGetUniformLocation(m_animation_program_id, "view");
	glUniformMatrix4fv(view_uniform, 1, GL_FALSE, (float*)&camera.m_view);


	int anim_diffuse_location = glGetUniformLocation(m_animation_program_id, "diffuse");
	glUniform1i(anim_diffuse_location, 0);

	FBXSkeleton *skeleton = m_file->getSkeletonByIndex(0);

	UpdateBones(skeleton);

	int bones_uniform = glGetUniformLocation(m_animation_program_id, "bones");
	glUniformMatrix4fv(bones_uniform, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);

	for (unsigned int i = 0; i < m_meshes.size(); ++i)
	{
		FBXMeshNode* curr_mesh = m_file->getMeshByIndex(i);

		mat4 world_transform = curr_mesh->m_globalTransform;
		FBXMaterial* mesh_material = curr_mesh->m_material;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh_material->textures[FBXMaterial::DiffuseTexture]->handle);


		int world_uniform = glGetUniformLocation(m_animation_program_id, "world");
		glUniformMatrix4fv(world_uniform, 1, GL_FALSE, (float*)&world_transform);




		glBindVertexArray(m_meshes[i].m_VAO);
		glDrawElements(GL_TRIANGLES, m_meshes[i].m_index_count, GL_UNSIGNED_INT, 0);
	}
#pragma endregion

	//////////////////////////////


	// Light
	glBindFramebuffer(GL_FRAMEBUFFER, m_light_fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	
	glUseProgram(m_directional_light_program);

	int position_tex_uniform = glGetUniformLocation(m_directional_light_program, "position_tex");
	int normals_tex_uniform = glGetUniformLocation(m_directional_light_program, "normals_tex");

	glUniform1i(position_tex_uniform, 0);
	glUniform1i(normals_tex_uniform, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_position_texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normals_texture);

	///
	//renderDirectionalLight(vec3(1, 0, 0), vec3(1, 1, 1), camera);
	//renderDirectionalLight(vec3(0, -1, 0), vec3(1, 1, 1), camera);
	//renderDirectionalLight(vec3(0, 0, 1), vec3(1, 1, 1), camera);
	

	glUseProgram(m_point_light_program);
	view_proj_uniform = glGetUniformLocation(m_point_light_program, "proj_view");

	position_tex_uniform = glGetUniformLocation(m_point_light_program, "position_texture");
	normals_tex_uniform = glGetUniformLocation(m_point_light_program, "normal_texture");

	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE, (float*)&camera.m_projView);
	glUniform1i(position_tex_uniform, 0);
	glUniform1i(normals_tex_uniform, 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_position_texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normals_texture);

	//draw the point lights
	
	renderPointLight(vec3(1, 8, 0), 15, vec3(0, 1, 1), camera);
	renderPointLight(lightpos, 150, lightcolour, camera);
	renderPointLight(vec3(0, 0, 1), 15, vec3(1, 1, 0), camera);

	glDisable(GL_BLEND);
	// Composite
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.3, 0.3, 0.3, 1);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_composite_program);

	int albedo_tex_uniform = glGetUniformLocation(m_composite_program, "albedo_tex");
	int light_tex_uniform = glGetUniformLocation(m_composite_program, "light_tex");

	glUniform1i(albedo_tex_uniform, 0);
	glUniform1i(light_tex_uniform, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_albedo_texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_light_texture);


	glBindVertexArray(m_screenspace_quad.m_VAO);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glDisable(GL_CULL_FACE);
	//////////////////////////////////



	if (m_draw_gizmos)
	{
		Gizmos::draw(camera.getProjectionView());
	}

	TwDraw();
	m_emitter.Draw(m_time, camera.getWorldTransform(), camera.getProjectionView(), particle_pos);
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void Assignment::GenerateGLMeshes(FBXFile* fbx)
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
		glEnableVertexAttribArray(2); //bone indices
		glEnableVertexAttribArray(3); //bone weights
		glEnableVertexAttribArray(4); //normals


		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::PositionOffset);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::TexCoord1Offset);

		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::IndicesOffset);

		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::WeightsOffset);

		glVertexAttribPointer(4, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), (void*)FBXVertex::NormalOffset);


		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	}



}

void Assignment::EvaluateSkeleton(FBXAnimation* anim, FBXSkeleton* skeleton, float timer)
{
	float fps = 24.f;
	int current_frame = (int)(timer * fps);
	// loop through all the tracks
	for (unsigned int track_index = 0; track_index < anim->m_trackCount; ++track_index)
	{


		//wrap back to the start if we've gone too far
		int track_frame_count = anim->m_tracks[track_index].m_keyframeCount;
		int track_frame = current_frame % track_frame_count;

		//find what key frames are currently affecting the bone
		FBXKeyFrame curr_frame = anim->m_tracks[track_index].m_keyframes[track_frame];
		FBXKeyFrame next_frame = anim->m_tracks[track_index].m_keyframes[(track_frame + 1) % track_frame_count];


		//interpolate between those key frames to generate the matrix
		//for the current pose
		float time_since_frame_flip = timer - (current_frame / fps);
		float t = time_since_frame_flip * fps;

		vec3 new_pos = glm::mix(curr_frame.m_translation, next_frame.m_translation, t);
		vec3 new_scale = glm::mix(curr_frame.m_scale, next_frame.m_scale, t);
		glm::quat new_rot = glm::slerp(curr_frame.m_rotation, next_frame.m_rotation, t);

		mat4 local_transform = glm::translate(new_pos) * glm::toMat4(new_rot) * glm::scale(new_scale);

		// get the right track for the given bone
		unsigned int bone_index = anim->m_tracks[track_index].m_boneIndex;

		//set the FBXnode's local transform to match
		if (bone_index < skeleton->m_boneCount)
		{
			skeleton->m_nodes[bone_index]->m_localTransform = local_transform;
		}

	}
}

void Assignment::UpdateBones(FBXSkeleton* skeleton)
{
	//loop through the nodes in the skeleton
	for (unsigned int bone_index = 0; bone_index < skeleton->m_boneCount; ++bone_index)
	{
		//generate their global transforms
		int parent_index = skeleton->m_parentIndex[bone_index];
		if (parent_index == -1)
		{
			skeleton->m_bones[bone_index] = skeleton->m_nodes[bone_index]->m_localTransform;
		}
		else
		{
			skeleton->m_bones[bone_index] = skeleton->m_bones[parent_index] * skeleton->m_nodes[bone_index]->m_localTransform;
		}

		//multiply their global transforms by the inverse bind pose

	}

	for (unsigned int bone_index = 0; bone_index < skeleton->m_boneCount; ++bone_index)
	{
		skeleton->m_bones[bone_index] = skeleton->m_bones[bone_index] * skeleton->m_bindPoses[bone_index];
	}
}



void Assignment::buildMesh()
{
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	tinyobj::LoadObj(shapes, materials, "./models/stanford/dragon.obj");

	m_bunny.m_index_count = shapes[0].mesh.indices.size();

	tinyobj::mesh_t* mesh = &shapes[0].mesh;

	std::vector<float> vertex_data;
	vertex_data.reserve(mesh->positions.size() + mesh->normals.size());

	vertex_data.insert(vertex_data.end(), mesh->positions.begin(), mesh->positions.end());
	vertex_data.insert(vertex_data.end(), mesh->normals.begin(), mesh->normals.end());

	glGenVertexArrays(1, &m_bunny.m_VAO);
	glBindVertexArray(m_bunny.m_VAO);

	glGenBuffers(1, &m_bunny.m_VBO);
	glGenBuffers(1, &m_bunny.m_IBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_bunny.m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bunny.m_IBO);


	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* vertex_data.size(), vertex_data.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* mesh->indices.size(), mesh->indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float)* mesh->positions.size()));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



}

void Assignment::buildGBuffer()
{
	//create the frame buffer
	glGenFramebuffers(1, &m_gbuffer_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gbuffer_fbo);

	//generate all our textures
	//albedo, position, normal, depth(depth is render buffer)
	glGenTextures(1, &m_albedo_texture);
	glBindTexture(GL_TEXTURE_2D, m_albedo_texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenTextures(1, &m_position_texture);
	glBindTexture(GL_TEXTURE_2D, m_position_texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenTextures(1, &m_normals_texture);
	glBindTexture(GL_TEXTURE_2D, m_normals_texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenRenderbuffers(1, &m_gbuffer_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_gbuffer_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1280, 720);


	// attach our texturs to the frame buffer

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_albedo_texture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_position_texture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, m_normals_texture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_gbuffer_depth);

	GLenum targets[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

	glDrawBuffers(3, targets);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Error in creating gbuffer!\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//check that it worked
}

void Assignment::buildLightBuffer()
{
	//create fbo
	glGenFramebuffers(1, &m_light_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_light_fbo);
	//create textures
	//light texture
	glGenTextures(1, &m_light_texture);
	glBindTexture(GL_TEXTURE_2D, m_light_texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//attach textures
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_light_texture, 0);

	GLenum light_target = GL_COLOR_ATTACHMENT0;
	glDrawBuffers(1, &light_target);

	//check for sucess
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Error - light, framebuffer incorrect /n");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

void Assignment::buildQuad()
{
	vec2 half_texel = 1.0f / vec2(1280, 720) * 0.5f;

	float vertex_data[]
	{
		-1, -1, 0, 1, half_texel.x, half_texel.y,
			1, -1, 0, 1, 1 - half_texel.x, half_texel.y,
			1, 1, 0, 1, 1 - half_texel.x, 1 - half_texel.y,
			-1, 1, 0, 1, half_texel.x, 1 - half_texel.y,
	};

	unsigned int index_data[] =
	{
		0, 1, 2, 0, 2, 3
	};

	glGenVertexArrays(1, &m_screenspace_quad.m_VAO);

	glGenBuffers(1, &m_screenspace_quad.m_VBO);
	glGenBuffers(1, &m_screenspace_quad.m_IBO);

	glBindVertexArray(m_screenspace_quad.m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_screenspace_quad.m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_screenspace_quad.m_IBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_data), index_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 6, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 6, (void*)(sizeof(float)* 4));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Assignment::buildCube()
{

	float vertex_data[]
	{
		-1, -1, 1, 1,
			1, -1, 1, 1,
			1, -1, -1, 1,
			-1, -1, -1, 1,

			-1, 1, 1, 1,
			1, 1, 1, 1,
			1, 1, -1, 1,
			-1, 1, -1, 1

	};

	unsigned int index_data[] =
	{
		4, 5, 0,
		5, 1, 0,
		5, 6, 1,
		6, 2, 1,
		6, 7, 2,
		7, 3, 2,
		7, 4, 3,
		4, 0, 3,
		7, 6, 4,
		6, 5, 4,
		0, 1, 3,
		1, 2, 3,
	};

	glGenVertexArrays(1, &m_light_cube.m_VAO);

	glGenBuffers(1, &m_light_cube.m_VBO);
	glGenBuffers(1, &m_light_cube.m_IBO);

	glBindVertexArray(m_light_cube.m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_light_cube.m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_light_cube.m_IBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_data), index_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //position

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 4, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void Assignment::renderDirectionalLight(vec3 light_dir, vec3 light_color, FlyCamera camera)
{
	//vec3 light_dir = vec3(0, -1, 0);
	vec4 viewspace_light_dir = camera.m_view * vec4(light_dir, 0);
	//vec3 light_color = vec3(1, 0, 1);

	int light_dir_uniform = glGetUniformLocation(m_directional_light_program, "light_dir");
	int light_color_uniform = glGetUniformLocation(m_directional_light_program, "light_color");

	glUniform3fv(light_dir_uniform, 1, (float*)&viewspace_light_dir);
	glUniform3fv(light_color_uniform, 1, (float*)&light_color);

	glBindVertexArray(m_screenspace_quad.m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Assignment::renderPointLight(vec3 position, float radius, vec3 light_color, FlyCamera camera)
{
	Gizmos::addSphere(position, 0.1f, 16, 16, vec4(1));
	vec4 view_space_pos = camera.m_view * vec4(position, 1);

	int pos_uniform = glGetUniformLocation(m_point_light_program, "light_position");
	int view_pos_uniform = glGetUniformLocation(m_point_light_program, "light_view_position");
	int light_diffuse_uniform = glGetUniformLocation(m_point_light_program, "light_diffuse");
	int light_radius_uniform = glGetUniformLocation(m_point_light_program, "light_radius");

	glUniform3fv(pos_uniform, 1, (float*)&position);
	glUniform3fv(view_pos_uniform, 1, (float*)&view_space_pos);
	glUniform3fv(light_diffuse_uniform, 1, (float*)&light_color);
	glUniform1f(light_radius_uniform, radius);

	glBindVertexArray(m_light_cube.m_VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}