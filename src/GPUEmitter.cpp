#include "GPUEmitter.h"
#include "gl_core_4_4.h"
#include "Utility.h"

GPUPointEmitter::GPUPointEmitter() : m_particles(nullptr), m_max_particles(0), m_position(0, 0, 0),
m_draw_shader(0), m_update_shader(0), m_last_draw_time(0)
{
	m_vao[0] = 0;
	m_vao[1] = 0;
	m_vbo[0] = 0;
	m_vbo[1] = 0;
}
GPUPointEmitter::~GPUPointEmitter()
{
	delete[] m_particles;

	glDeleteVertexArrays(2, m_vao);
	glDeleteBuffers(2, m_vbo);

	//delete the shaders
	glDeleteProgram(m_draw_shader);
	glDeleteProgram(m_update_shader);


}

void GPUPointEmitter::Init(unsigned int a_max_particles, vec3 a_position,
	float a_emit_rate,
	float a_min_lifespan, float a_max_lifespan,
	float a_min_velocity, float a_max_velocity,
	float a_start_size, float a_end_size,
	vec4 a_start_color, vec4 a_end_color)
{
	m_start_color = a_start_color;
	m_end_color = a_end_color;
	m_start_size = a_start_size;
	m_end_size = a_end_size;
	m_velocity_min = a_min_velocity;
	m_velocity_max = a_max_velocity;
	m_lifespan_min = a_min_lifespan;
	m_lifespan_max = a_max_lifespan;
	m_max_particles = a_max_particles;

	//create particle array
	m_particles = new GPUParticle[a_max_particles];

	//set our starting ping-pong buffer
	m_active_buffer = 0;

	CreateBuffers();
	CreateUpdateShader();
	CreateDrawShader();
}


void GPUPointEmitter::CreateBuffers()
{
	//create opengl buffers
	glGenVertexArrays(2, m_vao);
	glGenBuffers(2, m_vbo);

	for (unsigned int buffer_index = 0; buffer_index < 2; ++buffer_index)
	{
		glBindVertexArray(m_vao[buffer_index]);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[buffer_index]);
		glBufferData(GL_ARRAY_BUFFER, m_max_particles * sizeof(GPUParticle), m_particles, GL_STREAM_DRAW);

		glEnableVertexAttribArray(0);//position
		glEnableVertexAttribArray(1);//velocity
		glEnableVertexAttribArray(2);//lifetime
		glEnableVertexAttribArray(3);//lifespan


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), (void*)12);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), (void*)24);
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), (void*)28);

	}
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


}
void GPUPointEmitter::CreateUpdateShader()
{
	unsigned int vertex_shader;
	LoadShaderType("./data/shaders/particle_update_vert.glsl", GL_VERTEX_SHADER, &vertex_shader);

	m_update_shader = glCreateProgram();
	glAttachShader(m_update_shader,vertex_shader);

	const char* outputs[] = { "updated_position",
							"updated_velocity",
							"updated_lifetime",
							"updated_lifespan" };

	glTransformFeedbackVaryings(m_update_shader, 4, outputs, GL_INTERLEAVED_ATTRIBS);


	glLinkProgram(m_update_shader);
	glDeleteShader(vertex_shader);
}
void GPUPointEmitter::CreateDrawShader()
{
	LoadShader("./data/shaders/gpu_particle_vert.glsl", "./data/shaders/gpu_particle_geom.glsl", "./data/shaders/gpu_particle_frag.glsl", &m_draw_shader);
}


void GPUPointEmitter::Draw(float time, mat4 a_camera_transform, mat4 a_projection_view)
{
	//uniform float delta_time;
	//uniform vec3 emitter_position;
	//uniform float min_velocity;
	//uniform float max_velocity;
	//uniform float min_lifespan;
	//uniform float max_lifespan;
	//uniform float time;


	//update vertex pass
	glUseProgram(m_update_shader);

	int delta_uniform = glGetUniformLocation(m_update_shader, "delta_time");
	glUniform1f(delta_uniform, time - m_last_draw_time);

	int emitter_pos_uniform = glGetUniformLocation(m_update_shader, "emitter_position");
	glUniform3fv(emitter_pos_uniform, 1, (float*)&m_position);

	int min_vel_uniform = glGetUniformLocation(m_update_shader, "min_velocity");
	glUniform1f(min_vel_uniform, m_velocity_min);

	int max_vel_uniform = glGetUniformLocation(m_update_shader, "max_velocity");
	glUniform1f(max_vel_uniform, m_velocity_max);

	int min_life_uniform = glGetUniformLocation(m_update_shader, "min_lifespan");
	glUniform1f(min_life_uniform, m_lifespan_min);

	int max_life_uniform = glGetUniformLocation(m_update_shader, "max_lifespan");
	glUniform1f(max_life_uniform, m_lifespan_max);

	int time_uniform = glGetUniformLocation(m_update_shader, "time");
	glUniform1f(time_uniform, time);

	glEnable(GL_RASTERIZER_DISCARD);

	glBindVertexArray(m_vao[m_active_buffer]);

	unsigned int other_buffer = (m_active_buffer + 1) % 2;

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_vbo[other_buffer]);

	glBeginTransformFeedback(GL_POINTS);

	glDrawArrays(GL_POINTS, 0, m_max_particles);

	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
	
	//render pass
	glUseProgram(m_draw_shader);

	//uniform mat4 projection_view;
	//uniform mat4 camera_world;
	//
	//uniform float size_start;
	//uniform float size_end;
	//
	//uniform vec4 color_start;
	//uniform vec4 color_end;

	int proj_view_uniform = glGetUniformLocation(m_draw_shader, "projection_view");
	int camera_world_uniform = glGetUniformLocation(m_draw_shader, "camera_world");
	int size_start_uniform = glGetUniformLocation(m_draw_shader, "size_start");
	int size_end_uniform = glGetUniformLocation(m_draw_shader, "size_end");
	int color_start_uniform = glGetUniformLocation(m_draw_shader, "color_start");
	int color_end_uniform = glGetUniformLocation(m_draw_shader, "color_end");

	glUniformMatrix4fv(proj_view_uniform, 1, GL_FALSE, (float*)&a_projection_view);
	glUniformMatrix4fv(camera_world_uniform, 1, GL_FALSE, (float*)&a_camera_transform);
	
	glUniform1f(size_start_uniform,m_start_size);
	glUniform1f(size_end_uniform, m_end_size);
	glUniform4fv(color_start_uniform, 1, (float*)&m_start_color);
	glUniform4fv(color_end_uniform, 1, (float*)&m_end_color);

	glBindVertexArray(m_vao[other_buffer]);
	glDrawArrays(GL_POINTS, 0, m_max_particles);
	m_active_buffer = other_buffer;

	glPointSize(100);

	m_last_draw_time = time;
}
