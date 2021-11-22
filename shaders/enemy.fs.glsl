#version 330

// From vertex shader
in vec2 texcoord;
in vec2 vpos; // Distance from local origin
in vec2 world_pos;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform float color_scale;
uniform vec3 ambient_light;
uniform vec2 light_source_pos;
uniform vec3 light_col;
uniform float light_intensity;
uniform int in_shop;
uniform int inInvin;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	float reddenFactor = 0.5;
	color = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
	color = vec4(color.r + reddenFactor * color_scale, color.g, color.b, color.a);

	if (in_shop == 1) {
		if(color.a < 1.0)
			discard;
		float distance = distance(light_source_pos, world_pos);
		float diffuse = 0.0;
		if (distance <= light_intensity)
			diffuse =  1.0 - abs(distance / light_intensity);
		color = vec4(min(color.xyz * ((light_col * diffuse) + ambient_light), color.xyz), color.a);
	}

	float radius = distance(vec2(0.0), vpos);
	if (inInvin == 1 && radius < 0.3)
	{
		color.xyz += (0.3 - radius) * vec3(1.0, 1.0, 0.0);
	}
}