#version 330 core

layout(location = 0) in vec2 vertexPosition; // [0,1]x[0,1]
layout(location = 1) in vec4 vertexInstance; // vec4(x, y, code, color_index)

uniform sampler2D sampler_metadata;
uniform vec2 resolution_bitmap;
uniform vec2 resolution_window;
uniform float num_glyphs;

uniform vec2 string_offset;
uniform float offset_firstline;
uniform float scale_factor;

out vec2 uv;
out float color_index;

void main()
{
    // (xoff, yoff, xoff2-xoff, yoff2-yoff), from second row of texture
    vec4 q2 = texture(sampler_metadata, vec2((vertexInstance.z + 0.5)/num_glyphs, 0.75));

    // scale offsets to pixels:
    q2 *= vec4(resolution_bitmap, resolution_bitmap);
    
    // vertexPosition is a [0,1]x[0,1] square ("model space")
    vec2 p = vertexPosition*q2.zw + q2.xy;           // scale and offset properly relative to baseline
    p *= vec2(1.0, -1.0);                            // flip y, since texture is upside-down
    p.y -= offset_firstline;                         // make sure the upper-left corner of the string is in the upper-left corner of the screen
    p *= scale_factor;                               // scale relative to font size
    p += vertexInstance.xy + string_offset;          // move glyph into the right position
    p *= 2.0/resolution_window;                      // to NDC
    p += vec2(-1.0, 1.0);                            // move to upper-left corner instead of center

    // put it in place
    gl_Position = vec4(p, 0.0, 1.0);

    // (x0, y0, x1-x0, y1-y0), from first row of texture
    vec4 q = texture(sampler_metadata, vec2((vertexInstance.z + 0.5)/num_glyphs, 0.25));

    // pass the uv's
    uv = q.xy + vertexPosition*q.zw;

    // pass on the color index to the fragment shader
    color_index = vertexInstance.w; // fourth component
}
