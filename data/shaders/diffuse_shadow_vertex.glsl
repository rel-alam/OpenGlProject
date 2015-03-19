#version 410

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec4 frag_normal;

uniform mat4 view_projection;

void main()
{
	frag_normal = vec4(normal, 0);
	gl_Position = view_projection * vec4(position, 1);
}