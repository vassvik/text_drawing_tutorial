#version 330 core

uniform vec2 p0; // lower left corner
uniform vec2 p1; // upper right corner

uniform vec2 uv0; // lower left corner
uniform vec2 uv1; // upper right corner

out vec2 uv;

void main()
{
	// expand gl_VertexID into unit square positions of the triangle strip vertices
	float x = gl_VertexID / 2;
	float y = gl_VertexID % 2;
	vec2 p = vec2(x, y);

	// use uniforms to put into place
    gl_Position = vec4(p0 + p*(p1-p0), 0.0, 1.0);
    uv = uv0 + p*(uv1-uv0);
}
