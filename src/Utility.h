#ifndef ULTILITY_H
#define ULTILITY_H
#include "AntTweakBar.h"
#include "glm_header.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>

typedef unsigned int Gluint;

bool LoadShader(char* vertex_filename, 
				char* geometry_filename, 
				char* fragment_filename, 
				Gluint* result);

bool LoadShaderType(char* filename,
	GLenum shader_type,
	unsigned int* output);




void OnMouseButton(GLFWwindow* window, int button, int pressed, int mod_keys);

void OnMousePosition(GLFWwindow* window, double x, double y);

void OnMouseScroll(GLFWwindow* window, double x, double y);

void OnKey(GLFWwindow* window, int key, int scancode, int pressed, int mod_keys);
void OnChar(GLFWwindow* window, unsigned int c);
void OnWindowResize(GLFWwindow*, int width, int height);

#endif // !ULTILITY_H
