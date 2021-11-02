#version 330

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat3 transform;
uniform mat3 projection;
uniform int xFrame;
uniform int yFrame;

void main()
{
	float xScale = 1.0 / 9.0;
	float yScale = 1.0 / 4.0;
	texcoord = in_texcoord;
	texcoord.x = texcoord.x * xScale;
	texcoord.x += xScale * xFrame;
	texcoord.y = texcoord.y * yScale;
	texcoord.y += yScale * yFrame;
	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}