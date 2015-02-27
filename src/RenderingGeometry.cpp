#include "RenderingGeometry.h"
#include "glm_header.h"
#include "Vertex.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Utility.h"
bool RenderingGeometry::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}

	//generateShader();
	generateGrid(10, 10);

	LoadShader("./shaders/basic_vertex.glsl", 0,"./shaders/basic_fragment.glsl", &m_program_id);


	Gizmos::create();
	m_camera = new FlyCamera();
	m_camera->setLookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	m_camera->setSpeed(1);
	m_camera->setPrespective(60, 1280 / 720, 0.1f, 1000.f);
	timer = 0;

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	return true;
}
void RenderingGeometry::shutdown()
{
	Application::shutdown();
}
bool RenderingGeometry::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0f);

	Gizmos::clear();

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

	m_camera->update(dt);

	return true;
}
void RenderingGeometry::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	Gizmos::draw(m_camera->getProjectionView());

	glUseProgram(m_program_id);

	int proj_view_handle = glGetUniformLocation(m_program_id, "projection_view");

	if (proj_view_handle >= 0)
	{
		glUniformMatrix4fv(proj_view_handle, 1, false, (float*)&m_camera->getProjectionView());
	}
	timer+= 0.01f;
	
	int timer_handle = glGetUniformLocation(m_program_id, "timer");
	
	if (timer_handle >= 0)
	{
		glUniform1fv(timer_handle, 1, (float*)&timer);
	}
	height = 3;
	int height_handle = glGetUniformLocation(m_program_id, "heightScale");

	if (height_handle >= 0)
	{
		glUniform1fv(height_handle, 1, (float*)&height);
	}
	


	glBindVertexArray(m_VAO);
	
	glDrawElements(GL_TRIANGLES, m_index_count, GL_UNSIGNED_INT, 0);

	GLFWwindow* window = glfwGetCurrentContext();

	Application::draw();
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void RenderingGeometry::generateGrid(unsigned int rows, unsigned int cols)
{
	Vertex* vertex_array = new Vertex[(rows + 1) * (cols + 1)];
	for (unsigned int r = 0; r < rows + 1; ++r)
	{
		for (unsigned int c = 0; c < cols + 1; ++c)
		{
			vertex_array[c + r * (cols + 1)].position = vec4((float)c, 0, (float)r, 1);
			vertex_array[c + r * (cols+1)].color = vec4((float)c / (cols + 1), 0, (float)r / rows, 1);
		}
	}

	m_index_count = rows * cols * 6;
	unsigned int *index_array = new unsigned int[m_index_count];

	int index_location = 0;

	for (unsigned int r = 0; r < rows; ++r)
	{
		for (unsigned int c = 0; c < cols; ++c)
		{
			index_array[index_location + 0] = c + r * (cols+1);
			index_array[index_location + 1] = c + (r + 1) * (cols+1);
			index_array[index_location + 2] = (c + 1) + r * (cols+1);

			index_array[index_location + 3] = (c + 1) + r * (cols+1);
			index_array[index_location + 4] = c + (r + 1) * (cols+1);
			index_array[index_location + 5] = (c + 1) + (r + 1) * (cols+1);

			index_location += 6;
		}
	}


	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);
	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, (cols + 1)*(rows + 1) * sizeof(Vertex), vertex_array, GL_STATIC_DRAW);


	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec4));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, rows * cols * 6 * sizeof(unsigned int), index_array, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	

	delete[] vertex_array;
	delete[] index_array;
}


//void RenderingGeometry::generateShader()
//{
//
//	const char* vs_source =	"#version 410\n"
//								"layout(location=0) in vec4 position;\n"
//								"layout(location=1) in vec4 color;\n"
//								"out vec4 out_color;\n"
//								"uniform mat4 projection_view;\n"
//								"uniform float timer;"
//								"uniform float heightScale;"
//								"void main()\n"
//								"{\n"
//									"out_color = color;\n"
//									"vec4 pos = position;"
//									"pos.y = (sin(pos.x* timer) + sin(pos.z * timer) * heightScale);"
//									"gl_Position = projection_view * pos;\n"
//								"}\n";
//
//	const char* fs_source = "#version 410\n \
//							in vec4 out_color;\n \
//							out vec4 frag_color;\n \
//							void main() { frag_color = out_color;}";
//
//
//	unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
//	unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
//
//	glShaderSource(vertex_shader, 1, &vs_source, 0);
//	glCompileShader(vertex_shader);
//
//	glShaderSource(fragment_shader, 1, &fs_source, 0);
//	glCompileShader(fragment_shader);
//	int success = GL_FALSE;
//	
//	glGetShaderiv(m_program_id, GL_COMPILE_STATUS, &success);
//	if (success == GL_FALSE)
//	{
//		int log_length = 0;
//		glGetShaderiv(m_program_id, GL_INFO_LOG_LENGTH, &log_length);
//		char* log = new char[log_length];
//		glGetProgramInfoLog(m_program_id, log_length, 0, log);
//
//		printf("Error: Stuff broke in Linker!");
//		printf("%s\n", log);
//
//		delete[] log;
//
//	}
//
//
//	m_program_id = glCreateProgram();
//	glAttachShader(m_program_id, vertex_shader);
//	glAttachShader(m_program_id, fragment_shader);
//	glLinkProgram(m_program_id);
//
//	
//	glGetProgramiv(m_program_id, GL_LINK_STATUS, &success);
//	
//	if (success == GL_FALSE)
//	{
//		int log_length = 0;
//		glGetProgramiv(m_program_id, GL_INFO_LOG_LENGTH, &log_length);
//		char* log = new char[log_length];
//		glGetProgramInfoLog(m_program_id, log_length, 0, log);
//
//		printf("Error: Stuff broke in shader!");
//		printf("%s\n", log);
//
//		delete[] log;
//
//	}
//
//	glDeleteShader(fragment_shader);
//	glDeleteShader(vertex_shader);
//}