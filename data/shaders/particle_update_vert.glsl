#version 410


//move particles based on velocity
//update the lifetime based on delta time
//spawn new particles if we need to


layout(location=0) in vec3 position;
layout(location=1) in vec3 velocity;
layout(location=2) in float lifetime;
layout(location=3) in float lifespan;


out vec3 updated_position;
out vec3 updated_velocity;
out float updated_lifetime;
out float updated_lifespan;

const float INVERSE_MAX_UNIT = 1.0f / 4294967295.0f;

float rand(uint seed, float range)
{
	uint i = (seed ^ 12345391u) * 2654435769u;
	i ^= (i << 6u) ^ (i >> 26u);
	i *= 2654435769u;
	i += ( i << 5u) ^ (i >> 12u);
	return float(range * i) * INVERSE_MAX_UNIT;
}

uniform float delta_time;
uniform vec3 emitter_position;

uniform float min_velocity;
uniform float max_velocity;
uniform float min_lifespan;
uniform float max_lifespan;

uniform float time;


void main()
{
	//move particles based on velocity;
	updated_position = position + velocity * delta_time;
	updated_velocity = velocity;
	updated_lifespan = lifespan;

	//update the lifetime based on delta time;
	updated_lifetime = lifetime + delta_time;

	//spawn new particles if we need to
	
	if(updated_lifetime > updated_lifespan)
	{
		updated_position = emitter_position;
		uint seed = uint(time * 1000.0) + uint(gl_VertexID);

		//set its velocity
		float vel_range = max_velocity - min_velocity;		
		float vel_len = rand(seed++, vel_range) + min_velocity;

		updated_velocity.x = rand(seed++, 2) - 1;
		updated_velocity.y = rand(seed++, 2) - 1;
		updated_velocity.z = rand(seed++, 2) - 1;
		updated_velocity = normalize(updated_velocity) * vel_len;

		updated_lifetime = 0;
		
		//set its lifespan
		float lifespan_range = max_lifespan - min_lifespan;
		updated_lifespan = rand(seed++, lifespan_range) + min_lifespan;
	}


}



