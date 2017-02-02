#version 330 core

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 vertexUV;

out vec2 uv;

void main()
{
	vec2 p = vertexPosition;
	p.x *= 480/640.0;
    gl_Position = vec4(p, 0.0, 1.0);
    uv = vertexUV;
}
