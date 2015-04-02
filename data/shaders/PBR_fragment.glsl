#version 410

in vec4 frag_normal;

in vec4 frag_position;

out vec4 frag_color;

uniform vec3 ambient_light;

uniform vec3 light_dir;
uniform vec3 light_color;

uniform vec3 material_color;

uniform vec3 eye_pos;
uniform float specular_power;

uniform float roughness;

uniform float FresnelScale;

void main()
{
	vec3 ambient = material_color * ambient_light;
	//
	vec3 N = normalize(frag_normal.xyz);
	vec3 L = normalize(light_dir);
	//
	float d = max(0.0, dot(N, -L));
   vec3 diffuse = vec3(d) * light_color * material_color;
	//
	vec3 E = normalize(eye_pos - frag_position.xyz);
	vec3 R = reflect(L, N);
	//
	//float s = max(0,dot(R, E));
	//s = pow(s, specular_power);
	//vec3 specular = vec3(s) * light_color * material_color;
	//
	//frag_color = vec4(ambient + diffuse + specular,1);

	float R2 = roughness * roughness;
	vec3 H = normalize ( L + E);
	float NdH = max ( 0.0f, dot(N, H));
	float NdH2 = NdH * NdH;

	float NdL = max ( 0.0f, dot(N, L));
	float NdE = max ( 0.0f, dot(N, E));




	float e = 2.71828182845904523536028747135f;
	float pi = 3.1415926535897932384626433832f;


	// Beckman's Distribution Function D
	float exponent = -(1-NdH2) / (NdH2 * R2);
	float D = pow(e, exponent) / (R2  * NdH2 * NdH2);

	float HdE = max(0.0f, dot(H,E));
	//fresnel Term F
	float F = mix(pow(1-HdE, 5),1,FresnelScale);

	float X = 2.0f * NdH / dot(E,H);
	float G = min(1, min(X * NdL, X * NdE));


	float CookTorrance = max((D*G*F) / (NdE * pi), 0.0f);

	frag_color = vec4(ambient * (diffuse + CookTorrance), 1);

	







}