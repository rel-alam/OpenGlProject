#version 410

in vec2 frag_tex_coord;
out vec4 frag_color;
uniform sampler2D diffuse;


in vec3 fposition;
in vec3 fnormal;


layout(location = 0) out vec3 gPassAlbedo;
layout(location = 1) out vec3 gPassPosition;
layout(location = 2) out vec3 gPassNormal;

void main()
{
	gPassAlbedo = texture(diffuse, frag_tex_coord).rgb;
	gPassPosition = fposition.xyz;
	gPassNormal = fnormal.xyz;
}