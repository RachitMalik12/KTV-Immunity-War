#version 330

// From vertex shader
in vec2 texcoord;
in vec2 world_pos;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform float color_scale;
uniform vec3 ambient_light;
uniform vec2 light_source_pos;
uniform vec3 light_col;
uniform float light_intensity;
uniform int animationMode;
uniform int in_shop;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	color = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));

	if(animationMode == 3) {
		float darkenFactor = 0.3;
		color = vec4(color.r - darkenFactor * color_scale, color.g - darkenFactor * color_scale, color.b - darkenFactor * color_scale, color.a);
	}
	
	if(in_shop == 1) {
		if(color.a < 1.0)
			discard;
		float distance = distance(light_source_pos, world_pos);
		float diffuse = 0.0;
		if (distance <= light_intensity)
			diffuse =  1.0 - abs(distance / light_intensity);
		color = vec4(min(color.xyz * ((light_col * diffuse) + ambient_light), color.xyz), color.a);
	}
}
