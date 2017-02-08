#version 330 core

in float color_index;

out vec4 color;

void main()
{
	if (color_index < 0.5) {
		color = vec4(1.0, 0.0, 0.0, 1.0); // red
	} else if (color_index < 1.5) {
		color = vec4(0.0, 1.0, 0.0, 1.0); // green
	} else {
		color = vec4(1.0, 1.0, 1.0, 1.0); // white
	}
}