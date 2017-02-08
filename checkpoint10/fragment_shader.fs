#version 330 core

in float color_index;

uniform sampler1D sampler_colors;
uniform float num_colors;

out vec4 color;

void main()
{
	color = texture(sampler_colors, (color_index+0.5)/num_colors).rgba;
}