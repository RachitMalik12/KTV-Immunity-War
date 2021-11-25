#version 330

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;
out vec2 world_pos;

// Application data
uniform mat3 transform;
uniform mat3 projection;
uniform float time;

void main()
{
	texcoord = in_texcoord;
	vec3 pos = projection * transform * vec3(in_position.x, in_position.y, 1.0);
	gl_Position = vec4(pos.x, pos.y + 0.01 * sin(time), in_position.z, 1.0);
	world_pos = gl_Position.xy;
}