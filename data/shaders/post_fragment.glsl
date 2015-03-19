#version 410

in vec2 fTexCoord;

out vec4 FragColor;

uniform sampler2D target;

vec4 Simple()
{
	return texture(target, fTexCoord);
}

vec4 Distort()
{
	vec2 mid = vec2(0.5f);
	float distanceFromCentre = distance(fTexCoord, mid);
	vec2 normalizedCoord = normalize(fTexCoord - mid);
	float bias = distanceFromCentre + sin(distanceFromCentre * 15) * 0.02f;

	vec2 newCoord = mid + bias * normalizedCoord;
	return texture(target, newCoord);
}

vec4 BoxBlur()
{
	vec2 texel = 1.0f / textureSize(target, 0).xy;

	vec4 color = texture(target, fTexCoord);
	color += texture(target, fTexCoord + vec2(-texel.x, texel.y));
	color += texture(target, fTexCoord + vec2(-texel.x, 0));
	color += texture(target, fTexCoord + vec2(-texel.x, -texel.y));
	color += texture(target, fTexCoord + vec2(0, texel.y));
	color += texture(target, fTexCoord + vec2(0, -texel.y));
	color += texture(target, fTexCoord + vec2(texel.x, texel.y));
	color += texture(target, fTexCoord + vec2(texel.x, 0));
	color += texture(target, fTexCoord + vec2(texel.x, -texel.y));

	return color/9;

}

vec4 Sepia()
{
	vec4 color = texture(target, fTexCoord);
	vec4 newColor = new vec4(1,1,1,1);
	newColor.x = (color.x * .393) + (color.y * .769) + (color.z * .189);
	newColor.y = (color.x * .349) + (color.y * .686) + (color.z * .168);
	newColor.z = (color.x * .272) + (color.y * .534) + (color.z * .131);

	return newColor;
}

vec4 GuassianBlur()
{
	vec2 texel = 1.0f / textureSize(target, 0).xy;
	
	float quater = 0.25;
	float eigth = 0.5 * quater;

	float sixteenth = 0.5 * eigth;

	vec4 color = texture(target, fTexCoord) * quater;
	color += texture(target, fTexCoord + vec2(-texel.x, texel.y))* eigth;
	color += texture(target, fTexCoord + vec2(-texel.x, 0)) * eigth;
	color += texture(target, fTexCoord + vec2(-texel.x, -texel.y))*eigth;
	color += texture(target, fTexCoord + vec2(0, texel.y))*eigth;

	color += texture(target, fTexCoord + vec2(0, -texel.y))*sixteenth;
	color += texture(target, fTexCoord + vec2(texel.x, texel.y))*sixteenth;
	color += texture(target, fTexCoord + vec2(texel.x, 0))*sixteenth;
	color += texture(target, fTexCoord + vec2(texel.x, -texel.y))*quater;

	return color;

}

vec4 EdgeDetection()
{
	vec4 color = texture(target, fTexCoord);
	vec2 texel = 1.0f / textureSize(target, 0).xy;
	color += texture(target, fTexCoord + vec2(-texel.x, texel.y));
	color += texture(target, fTexCoord + vec2(-texel.x, 0));
	color += texture(target, fTexCoord + vec2(-texel.x, -texel.y));
	color += texture(target, fTexCoord + vec2(0, texel.y));

	return color;
}

void main()
{
	FragColor = EdgeDetection();
}