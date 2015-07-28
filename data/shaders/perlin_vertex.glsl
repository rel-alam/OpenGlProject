#version 410

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texcoord;

out vec2 frag_texcoord;
uniform mat4 view_proj;
uniform mat4 view;
uniform mat4 world;


uniform sampler2D perlin_texture;
uniform float scale;

out vec3 fPosition;
out vec3 fNormal;



 
void main()
{
	vec4 pos = position;
	pos.y += texture(perlin_texture, texcoord).r * scale;

	
	//grab the right position of current
	vec3 rightpos = position.xyz;
	rightpos.x += 0.5;
	rightpos.y += texture(perlin_texture, texcoord + vec2(0.0005f, 0)).r * scale;
	
	//grab bottom of current
	vec3 bottompos = position.xyz;
	bottompos.z += 0.5;
	bottompos.y += texture(perlin_texture, texcoord + vec2(0, 0.0005f)).r * scale;
	
	vec4 N = vec4(cross(bottompos, rightpos),0);
	//N.w = 0;
	N = normalize(N);

	frag_texcoord = texcoord;
	gl_Position = view_proj * pos;

	fPosition = (view * vec4(pos.xyz,1)).xyz;
	fNormal = (view * vec4(0,1,0,0)).xyz;

}