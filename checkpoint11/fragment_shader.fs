#version 330 core

in vec2 uv;
in float color_index;

uniform sampler2D sampler_bitmap;
uniform sampler1D sampler_colors;
uniform float num_colors;

out vec4 color;

void main()
{	
	vec3 col = texture(sampler_colors, (color_index+0.5)/num_colors).rgb;
	color = vec4(col*texture(sampler_bitmap, uv).rrr, 1.0);
}