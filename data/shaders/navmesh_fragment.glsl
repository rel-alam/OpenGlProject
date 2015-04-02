#version 410

in vec4 world_pos;
in vec4 world_normal;

out vec4 frag_color;

void main()
{
	vec3 color = vec3(1);

	if(mod(world_pos.x,1.0f) < 0.05f || mod(world_pos.y,1.0f) < 0.05f || mod(world_pos.z, 1.0f) < 0.05f)
	{
		color = vec3(0);
	}

	vec3 L = normalize(vec3(1));
	vec3 N = normalize(world_normal.xyz);
	float d = max(0, dot(N, L)) * 0.75f;

	frag_color.rgb = color * 0.25f + color * d;
	frag_color.a = 1;
}