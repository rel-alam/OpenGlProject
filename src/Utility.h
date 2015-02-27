#ifndef ULTILITY_H
#define ULTILITY_H

typedef unsigned int Gluint;

bool LoadShader(char* vertex_filename, 
				char* geometry_filename, 
				char* fragment_filename, 
				Gluint* result);

bool LoadShaderType(char* filename,
	GLenum shader_type,
	unsigned int* output);


#endif // !ULTILITY_H
