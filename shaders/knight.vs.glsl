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
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}