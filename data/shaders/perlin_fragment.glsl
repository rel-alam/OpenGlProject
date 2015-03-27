#version 410

in vec2 frag_texcoord;

out vec4 frag_color;

uniform sampler2D perlin_texture;

void main()
{

	float perlin_sample = texture(perlin_texture, frag_texcoord).r;
	//frag_color = texture(perlin_texture, frag_texcoord).rrrr;

	vec4 red = vec4(1, 0, 0, 1);
	vec4 blue = vec4(0, 0, 1, 1);
	vec4 green = vec4(0, 1, 0, 1);
	vec4 white = vec4(1,1, 1, 1);
	vec4 black = vec4(0, 0, 0, 1);

	if (perlin_sample > 1)
	{
		frag_color = white;
	}
	else if (perlin_sample > 0.9 && perlin_sample <= 1.2)
	{
		frag_color = mix(green, white, perlin_sample);
	}
	else if (perlin_sample > 0.7 && perlin_sample < 0.91)
	{
		frag_color = green;
	}
	else
	{
		frag_color = blue;
	}
	frag_color.a = 1;
	
}