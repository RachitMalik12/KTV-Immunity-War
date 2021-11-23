#version 330

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat3 transform;
uniform mat3 projection;
uniform int frame;

void main()
{
	float scale = 1.0 / 10.0;
	texcoord = in_texcoord;
	texcoord.x = texcoord.x * scale;
	texcoord.x += scale * frame;
	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}