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
uniform int in_shop;
uniform int inInvin;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	color = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
}