#version 410

in vec2 frag_texcoord;

out vec4 frag_color;

uniform sampler2D perlin_texture;
uniform sampler2D grass_texture;
uniform sampler2D water_texture;
uniform sampler2D snow_texture;

layout(location = 0) out vec3 gPassAlbedo;
layout(location = 1) out vec3 gPassPosition;
layout(location = 2) out vec3 gPassNormal;

in vec3 fPosition;
in vec3 fNormal;

void main()
{

	float perlin_sample = texture(perlin_texture, frag_texcoord).r;

	if (perlin_sample > 0.79)
	{
		gPassAlbedo = texture(snow_texture, frag_texcoord).rgb;
		gPassPosition = fPosition.xyz;
		gPassNormal = fNormal.xyz * 0.5 + 0.5f;
	}
	else if (perlin_sample > 0.4 && perlin_sample < 0.8)
	{
		gPassAlbedo = texture(grass_texture, frag_texcoord).rgb;
		gPassPosition = fPosition.xyz;
		gPassNormal = fNormal.xyz* 0.5 + 0.5f;
	}
	else
	{
		gPassAlbedo = texture(water_texture, frag_texcoord).rgb;
		gPassPosition = fPosition.xyz;
		gPassNormal = fNormal.xyz* 0.5 + 0.5f;
	}
	
}