#version 330 core

//Define colors.
uniform vec3 id;

//Define out variable for the fragment shader: color.
out vec4 color;

void main()
{
	color = vec4(id, 1.0);
}