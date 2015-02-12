#ifndef _VERTEX_H_
#define _VERTEX_H_

#include "glm_header.h"

struct Vertex
{
	vec4 position;
	vec4 color;
};

struct VertexTexCoord
{
	vec4 position;
	vec2 tex_coord;
};
#endif // !_VERTEX_H_
