#version 330 core

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in float vertexColorIndex;

out float color_index;

void main()
{
    gl_Position = vec4(0.5*vertexPosition, 0.0, 1.0);
    color_index = vertexColorIndex;
}
