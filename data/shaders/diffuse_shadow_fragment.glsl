#version 410

in vec4 frag_normal;

out vec4 frag_color;

uniform vec3 light_dir;

void main()
{
	float d = max(0, dot(-light_dir, normalize(frag_normal).xyz));
	frag_color = vec4(d,d,d,1);
}