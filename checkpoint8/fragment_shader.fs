#version 330 core

in vec2 uv;

uniform sampler2D texture_sampler;

out vec4 color;

void main()
{
	color = vec4(texture(texture_sampler, uv + vec2(-0.5/512, -0.5/256)).rrr, 1.0);
}