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

void main()
{
	vpos = in_position.xy; // local coordinated before transform
	texcoord = in_texcoord;
	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	if (inInvin == 1) {
		gl_Position = vec4(pos.x + 0.01 * cos(time * 5), pos.y + 0.01 * sin(time * 5),  in_position.z, 1.0);
	} else {
		gl_Position = vec4(pos.xy,  in_position.z, 1.0);
	}
	world_pos = gl_Position.xy;
}