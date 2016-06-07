#version 330 core

in vec3 FragPos;
in vec4 ParticleColor;

uniform samplerCube skybox;
uniform vec3 viewPos;

//Define out variable for the fragment shader: color.
out vec4 color;

void main()
{
	vec3 I = normalize(FragPos - viewPos);
	vec3 R = reflect(I, vec3(0.0, 1.0, 0.0));
	vec4 reflect_color = texture(skybox, R) * 0.8;
	color = reflect_color * length(ParticleColor);
}

