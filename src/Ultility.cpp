#include "gl_core_4_4.h"
#include <cstdio>


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