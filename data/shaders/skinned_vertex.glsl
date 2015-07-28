#version 410


layout(location=0) in vec4 position;
layout(location=1) in vec2 tex_coord;
layout(location=2) in vec4 bone_indices;
layout(location=3) in vec4 bone_weights;
layout(location=4) in vec4 normal;

out vec2 frag_tex_coord;

uniform mat4 projection_view;
uniform mat4 world;
uniform mat4 view;

const int MAX_BONES = 128;
uniform mat4 bones[MAX_BONES];

out vec3 fposition;
out vec3 fnormal;



void main()
{
	frag_tex_coord = tex_coord;

	ivec4 indices = ivec4(bone_indices);

	vec4 final_position = vec4(0);
	final_position += bones[indices.x] * position * bone_weights.x;
	final_position += bones[indices.y] * position * bone_weights.y;
	final_position += bones[indices.z] * position * bone_weights.z;
	final_position += bones[indices.w] * position * bone_weights.w;

	vec4 final_normal = vec4(0);
	final_normal += bones[indices.x] * normal * bone_weights.x;
	final_normal += bones[indices.y] * normal * bone_weights.y;
	final_normal += bones[indices.z] * normal * bone_weights.z;
	final_normal += bones[indices.w] * normal * bone_weights.w;

	final_position.w = 1;

	final_normal.w = 0;

	gl_Position = projection_view * world * final_position;

	fposition = (view * world * final_position).xyz;
	fnormal = (view * final_normal).xyz;

}