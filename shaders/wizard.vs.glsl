#version 330

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat3 transform;
uniform mat3 projection;
uniform int frameWalk;
uniform int frameIdle;
uniform int frameAttack;
uniform int animationMode;

void main()
{
	float walkScale = 1.0 / 2.0;
	float idleScale = 1.0 / 3.0;
	float attackScale = 1.0 / 4.0;
	texcoord = in_texcoord;
	if (animationMode == 0) {
		texcoord.x = texcoord.x * idleScale;
		texcoord.x += idleScale * frameIdle;
	} else if (animationMode == 1) {
		texcoord.x = texcoord.x * walkScale;
		texcoord.x += walkScale * frameWalk;
	} else {
		texcoord.x = texcoord.x * attackScale;
		texcoord.x += attackScale * frameAttack;
	}

	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}