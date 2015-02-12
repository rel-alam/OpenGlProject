#include "gl_core_4_4.h"
#include <cstdio>

bool LoadShader(char* vertex_filename, char* fragment_filename, GLuint* result)
{

	bool succeeded = false;

	FILE* vertex_file = fopen(vertex_filename, "r");
	FILE* fragment_file = fopen(fragment_filename, "r");

	if (vertex_file == 0 || fragment_file == 0)
	{
		fclose(vertex_file);
		fclose(fragment_file);
		return succeeded;
	}
	else
	{
		fseek(vertex_file, 0, SEEK_END);
		int vertex_file_length = ftell(vertex_file);
		fseek(vertex_file, 0, SEEK_SET);

		fseek(fragment_file, 0, SEEK_END);
		int fragment_file_length = ftell(fragment_file);
		fseek(fragment_file, 0, SEEK_SET);

		char* vs_source = new char[vertex_file_length];
		char* fs_source = new char[fragment_file_length];

		vertex_file_length = fread(vs_source, 1, vertex_file_length, vertex_file);
		fragment_file_length = fread(fs_source, 1, fragment_file_length, fragment_file);

		int log_length = 0;
		unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vertex_shader, 1, &vs_source, &vertex_file_length);
		glCompileShader(vertex_shader);

		glShaderSource(fragment_shader, 1, &fs_source, &fragment_file_length);
		glCompileShader(fragment_shader);


		*result = glCreateProgram();
		glAttachShader(*result, vertex_shader);
		glAttachShader(*result, fragment_shader);
		glLinkProgram(*result);

		int success = GL_FALSE;
		glGetProgramiv(*result, GL_LINK_STATUS, &success);

		if (success == GL_FALSE)
		{
			log_length = 0;
			glGetProgramiv(*result, GL_INFO_LOG_LENGTH, &log_length);
			char* log = new char[log_length];
			glGetProgramInfoLog(*result, log_length, 0, log);

			printf("Error: Stuff broke in shader!");
			printf("%s\n", log);

			delete[] log;

		}

		glDeleteShader(fragment_shader);
		glDeleteShader(vertex_shader);

		succeeded = true;

		delete[] vs_source;
		delete[] fs_source;
	}

	fclose(vertex_file);
	fclose(fragment_file);

	return succeeded;
}