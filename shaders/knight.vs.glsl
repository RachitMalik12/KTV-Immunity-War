#version 330
#define M_PI 3.1415926535897932384626433832795

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;
out vec2 world_pos;
out vec2 local_pos;

// Application data
uniform mat3 translate;
uniform mat3 rotation;
uniform mat3 scale;
uniform mat3 transform;
uniform mat3 projection;
uniform int xFrame;
uniform int yFrame;
uniform float time;
uniform int inInvin;
uniform int isDead;
uniform float animationTime;

vec4 deathAnimation() {
	mat3 newRotation = rotation;
	float radians = 90.0 * (M_PI / 180.0) * animationTime;
	float c = cos(radians);
	float s = sin(radians);
	newRotation[0][0] = c;
	newRotation[0][1] = s;
	newRotation[1][0] = -s;
	newRotation[1][1] = c;
	mat3 newTransform = translate * newRotation * scale;
	vec3 pos = projection * newTransform * vec3(in_position.xy, 1.0);
	return vec4(pos.xy,  in_position.z, 1.0);
}

void main()
{
	local_pos = in_position.xy;

	float xScale = 1.0 / 9.0;
	float yScale = 1.0 / 4.0;
	float xShift = xScale * 0.2;
	float yShift = yScale * 0.085;
	texcoord = in_texcoord;
	texcoord.x = texcoord.x * xScale * 0.6;
	texcoord.x += xScale * xFrame;
	texcoord.x += xShift;
	
	texcoord.y = texcoord.y * yScale * 0.9;
	texcoord.y += yScale * yFrame;
	texcoord.y += yShift;
	
	
	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	if (inInvin == 1) {
		float shakeDistance = 0.005;
		float shakeFrequencyModifier = 5.0;
		gl_Position = vec4(pos.x + shakeDistance * cos(time * shakeFrequencyModifier), pos.y + shakeDistance * sin(time * shakeFrequencyModifier),  in_position.z, 1.0);
	} else if (isDead == 1) {
		gl_Position = deathAnimation();
	}
	else {
		gl_Position = vec4(pos.xy,  in_position.z, 1.0);
	}
	world_pos = gl_Position.xy;
}