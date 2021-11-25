#version 330

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;
out vec2 vpos;
out vec2 world_pos;

// Application data
uniform mat3 transform;
uniform mat3 projection;
uniform float time;
uniform int inInvin;
uniform vec2 velocityOfPlayerHit;
uniform int playerDamage;
uniform int isDead;
uniform int gotCut;
uniform float animationTime;

void main()
{
	vpos = in_position.xy; // local coordinated before transform
	texcoord = in_texcoord;
	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	if (inInvin == 1) {
		float knockBackDistance = 0.1;
		float knockBackPlayerDamageModifier = 0.02;
		pos.x = pos.x + (sin(time) + 1.0) / 2 * (knockBackDistance * velocityOfPlayerHit.x + knockBackPlayerDamageModifier * playerDamage);
		pos.y = pos.y + (sin(time) + 1.0) / 2 * (knockBackDistance * velocityOfPlayerHit.y + knockBackPlayerDamageModifier * playerDamage);
		float shakeDistance = 0.01;
		float shakeFrequencyModifier = 5.0;
		gl_Position = vec4(pos.x + shakeDistance * cos(time * shakeFrequencyModifier), pos.y + shakeDistance * sin(time * shakeFrequencyModifier),  in_position.z, 1.0);
	} else if (isDead == 1) {
		if (gotCut == 1) {
			float cutDistance = 0.1;
			if (gl_VertexID == 0 || gl_VertexID == 3 || gl_VertexID == 1) {
				pos.x = pos.x - cutDistance * animationTime;
				pos.y = pos.y - cutDistance * animationTime;
			} else {
				pos.x = pos.x + cutDistance * animationTime;
				pos.y = pos.y + cutDistance * animationTime;
			}
			gl_Position = vec4(pos.xy,  in_position.z, 1.0);
		}
	}
	else {
		gl_Position = vec4(pos.xy,  in_position.z, 1.0);
	}
	world_pos = gl_Position.xy;
}