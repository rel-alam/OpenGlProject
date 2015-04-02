#version 410

layout(location=0) in vec3 position;
layout(location=0) in vec3 normal;

out vec4 world_pos;
out vec4 world_normal;

uniform mat4 proj_view;

void main()
{
	world_pos =vec4(position, 1);
	world_normal = vec4(normal, 0);

	gl_Position = proj_view * world_pos;
}