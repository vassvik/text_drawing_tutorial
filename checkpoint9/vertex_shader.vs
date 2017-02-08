#version 330 core

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec4 vertexInstance;

out float color_index;

void main()
{
	vec2 offset = vertexInstance.xy;
	
	// apply the offset that we sent in the instanced buffer
	vec2 p = offset + vertexPosition;

	// fix aspect ratio
	p.x *= 480/640.0; 

	// put it in place
    gl_Position = vec4(p, 0.0, 1.0);

    // pass on the color index to the fragment shader
    color_index = vertexInstance.w; // fourth component
}
