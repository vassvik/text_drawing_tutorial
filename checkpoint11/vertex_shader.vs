#version 330 core

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec4 vertexInstance;

uniform sampler2D sampler_metadata;
uniform vec2 resolution_bitmap;
uniform float num_glyphs; // not used

out vec2 uv;
out float color_index;

void main()
{

	/*
	// we'll apply this one in the next checkpoint
	// (xoff, yoff, xoff2, yoff2), from second row of texture
	vec4 q2 = texture(sampler_metadata, vec2((vertexInstance.z + 0.5)/num_glyphs, 0.75));

	// scale offsets to pixels:
	q2 *= vec4(resolution_bitmap, resolution_bitmap);
	*/

	vec2 offset = vertexInstance.xy;
	
	// apply the offset that we sent in the instanced buffer
	vec2 p = offset + vertexPosition;
	// fix aspect ratio
	p.x *= 480/640.0; 

	// put it in place
    gl_Position = vec4(p, 0.0, 1.0);


	// (x0, y0, x1-x0, y1-y0), from first row of texture
	vec4 q = texture(sampler_metadata, vec2((vertexInstance.z + 0.5)/num_glyphs, 0.25));

    // pass the uv's
    uv = q.xy + vertexPosition*q.zw;

    // pass on the color index to the fragment shader
    color_index = vertexInstance.w; // fourth component
}
