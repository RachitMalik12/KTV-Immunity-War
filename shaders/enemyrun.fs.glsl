#version 330

// From vertex shader
in vec2 texcoord;
in vec2 vpos; // Distance from local origin

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform int light_up;
uniform float light_up_scale;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	//color = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
	color = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
	float radius = distance(vec2(0.0), vpos);
	if (light_up == 1)
	{
		//color.xyz += (0.3 - radius) * vec3(light_up_scale, 0.0, 0.0);
		color.x = light_up_scale;
	}
	//color = color * 2;
}