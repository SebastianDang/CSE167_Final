#version 330 core

in vec4 ParticleColor;

//Define out variable for the fragment shader: color.
out vec4 color;

void main()
{
	color = ParticleColor;
}

