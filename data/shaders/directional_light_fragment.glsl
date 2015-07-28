#version 410


in vec2 frag_texcoord;

out vec3 out_color;

uniform vec3 light_dir;
uniform vec3 light_color;

uniform sampler2D position_tex;
uniform sampler2D normals_tex;

void main()
{
	vec3 normal_sample = texture(normals_tex, frag_texcoord).xyz;
	normal_sample *= 2;
	normal_sample -= 1;

	vec3 position_sample = texture(position_tex, frag_texcoord).xyz;

	//vec3 N = normalize(normal_sample);

	float d = max(0, dot(-light_dir, normal_sample));

	out_color = light_color * d;
	//out_color = N;
	//out_color = vec3(d);
	//out_color = light_color;
	//out_color = normal_sample;

}