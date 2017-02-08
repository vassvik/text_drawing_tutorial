#version 330 core

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec4 vertexInstance;

uniform sampler1D sampler_metadata;
uniform float num_glyphs; // not used

out float color_index;

void main()
{
	vec2 offset = vertexInstance.xy;
	
	// grab the scaling factor from the metadata texture
	vec2 scale = texture(sampler_metadata, (vertexInstance.z+0.5)/num_glyphs).xy;

	// apply the offset that we sent in the instanced buffer
	vec2 p = offset + scale*vertexPosition;

	// fix aspect ratio
	p.x *= 480/640.0; 

	// put it in place
    gl_Position = vec4(p, 0.0, 1.0);

    // pass on the color index to the fragment shader
    color_index = vertexInstance.w; // fourth component
}
