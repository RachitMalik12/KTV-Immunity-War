#version 330

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;
out vec2 local_coord;
out vec2 world_pos;

// Application data
uniform mat3 transform;
uniform mat3 projection;
uniform int frameWalk;
uniform int frameIdle;
uniform int frameAttack;
uniform int animationMode;
uniform float time;
uniform int inInvin;

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
		gl_Position = vec4(pos.x + 0.005 * cos(time * 5), pos.y + 0.01 * sin(time * 5),  in_position.z, 1.0);
	} else {
		gl_Position = vec4(pos.xy,  in_position.z, 1.0);
	}
	world_pos = gl_Position.xy;
}