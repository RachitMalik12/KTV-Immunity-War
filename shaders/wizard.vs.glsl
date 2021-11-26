#version 330
#define M_PI 3.1415926535897932384626433832795

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;
out vec2 local_coord;
out vec2 world_pos;

// Application data
uniform mat3 translate;
uniform mat3 rotation;
uniform mat3 scale;
uniform mat3 transform;
uniform mat3 projection;
uniform int frameWalk;
uniform int frameIdle;
uniform int frameAttack;
uniform int animationMode;
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
	local_coord = in_position.xy;
	float walkScale = 1.0 / 2.0;
	float idleScale = 1.0 / 3.0;
	float attackScale = 1.0 / 3.0;
	texcoord = in_texcoord;
	if (animationMode == 0) {
		texcoord.x = texcoord.x * idleScale;
		texcoord.x += idleScale * frameIdle;
	} else if (animationMode == 1) {
		texcoord.x = texcoord.x * walkScale;
		texcoord.x += walkScale * frameWalk;
	} else if (animationMode == 2) {
		texcoord.x = texcoord.x * attackScale;
		texcoord.x += attackScale * frameAttack;
	}

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