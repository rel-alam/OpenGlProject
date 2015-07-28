#include "gl_core_4_4.h"
#include <cstdio>
#include "AntTweakBar.h"
#include "glm_header.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Utility.h"
#include "tiny_obj_loader.h"
#include "stb_image.h"

void OnMouseButton(GLFWwindow* window, int button, int pressed, int mod_keys)
{
	TwEventMouseButtonGLFW(button, pressed);
}

void OnMousePosition(GLFWwindow* window, double x, double y)
{
	TwEventMousePosGLFW((int)x, (int)y);
}

void OnMouseScroll(GLFWwindow* window, double x, double y)
{
	TwEventMouseWheelGLFW((int)y);
}

void OnKey(GLFWwindow* window, int key, int scancode, int pressed, int mod_keys)
{
	TwEventKeyGLFW(key, pressed);
}

void OnChar(GLFWwindow* window, unsigned int c)
{
	TwEventCharGLFW(c, GLFW_PRESS);
}

void OnWindowResize(GLFWwindow*, int width, int height)
{
	TwWindowSize(width, height);
	glViewport(0, 0, width, height);
}


bool LoadShaderType(char* filename,
	GLenum shader_type, 
	unsigned int* output)
{

	bool succeeded = true;

	FILE* shader_file = fopen(filename, "r");

	//did it open successfully
	if (shader_file == 0)
	{
		succeeded = false;
	}

	else
	{
		// find out how long the file is
		fseek(shader_file, 0, SEEK_END);
		int shader_file_length = ftell(shader_file);
		fseek(shader_file, 0, SEEK_SET);

		//allocate enough space for the shader
		char *shader_source = new char[shader_file_length];
		//read the file anmd update the length to be accurate
		shader_file_length = fread(shader_source, 1, shader_file_length, shader_file);

		
		int log_length = 0;

		//create the shader handle
		unsigned int shader_handle = glCreateShader(shader_type);
		//compile the shader
		glShaderSource(shader_handle, 1, &shader_source, &shader_file_length);
		glCompileShader(shader_handle);
		//catch for errors
		int success = GL_FALSE;
		glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE)
		{
			glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &log_length);
			char* log = new char[log_length];
			glGetShaderInfoLog(shader_handle, log_length, NULL, log);
			printf("%s\n", log);
			delete[] log;
			succeeded = false;
		}
		if (succeeded)
		{
			*output = shader_handle;
		}
		delete[] shader_source;
		fclose(shader_file);

	}

	return succeeded;

}

bool LoadShader(char* vertex_filename,char* geometry_filename, char* fragment_filename, GLuint* result)
{
	bool succeeded = true;

	*result = glCreateProgram();

	unsigned int vertex_shader;

	LoadShaderType(vertex_filename, GL_VERTEX_SHADER, &vertex_shader);
	glAttachShader(*result, vertex_shader);
	glDeleteShader(vertex_shader);


	if (geometry_filename != nullptr)
	{
		unsigned int geometry_shader;
		LoadShaderType(geometry_filename, GL_GEOMETRY_SHADER, &geometry_shader);
		glAttachShader(*result, geometry_shader);
		glDeleteShader(geometry_shader);
	}

	if (fragment_filename != nullptr)
	{
		unsigned int fragment_shader;
		LoadShaderType(fragment_filename, GL_FRAGMENT_SHADER, &fragment_shader);
		glAttachShader(*result, fragment_shader);
		glDeleteShader(fragment_shader);
	}

		glLinkProgram(*result);

		GLint success;

		glGetProgramiv(*result, GL_LINK_STATUS, &success);

		if (success == GL_FALSE)
		{
			GLint log_length;
			glGetProgramiv(*result, GL_INFO_LOG_LENGTH, &log_length);

			char* log = new char[log_length];
			glGetProgramInfoLog(*result, log_length, 0, log);

			printf("ERROR: STUFF DONE SCREWED UP IN UR SHADER BUDDY!\n\n");
			printf("%s", log);

			delete[] log;
			succeeded = false;
		}
	return succeeded;
}


OpenGLData LoadOBJ(char* filename)
{
	OpenGLData result = {};
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	
	
	
	std::string err = tinyobj::LoadObj(shapes, materials, filename);

	if (err.size() != 0)
	{
		return result;
	}

	result.m_index_count = shapes[0].mesh.indices.size();

	tinyobj::mesh_t* mesh = &shapes[0].mesh;

	std::vector<float> vertex_data;
	vertex_data.reserve(mesh->positions.size() + mesh->normals.size());

	vertex_data.insert(vertex_data.end(), mesh->positions.begin(), mesh->positions.end());
	vertex_data.insert(vertex_data.end(), mesh->normals.begin(), mesh->normals.end());

	glGenVertexArrays(1, &result.m_VAO);
	glBindVertexArray(result.m_VAO);

	glGenBuffers(1, &result.m_VBO);
	glGenBuffers(1, &result.m_IBO);

	glBindBuffer(GL_ARRAY_BUFFER, result.m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result.m_IBO);


	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* vertex_data.size(), vertex_data.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* mesh->indices.size(), mesh->indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float)* mesh->positions.size()));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return result;
}

void loadTexture(const char* filename, unsigned int &m_texture)
{
	int width;
	int height;

	int channels;


	unsigned char* data = stbi_load(filename, &width, &height, &channels, STBI_default);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);
}
