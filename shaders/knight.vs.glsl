#version 330

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;
out vec2 world_pos;
out vec2 local_pos;

// Application data
uniform mat3 transform;
uniform mat3 projection;
uniform int xFrame;
uniform int yFrame;
uniform float time;
uniform int inInvin;

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
		gl_Position = vec4(pos.x + 0.005 * cos(time * 5), pos.y + 0.01 * sin(time * 5),  in_position.z, 1.0);
	} else {
		gl_Position = vec4(pos.xy,  in_position.z, 1.0);
	}
	world_pos = gl_Position.xy;
}