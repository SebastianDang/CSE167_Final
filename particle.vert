#version 330 core

//Define position, normal, and texture defined in the Container.
layout (location = 0) in vec3 vertex;

//Define uniform MVP: model, view, projection passed from the object.
uniform mat4 MVP;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 offset;
uniform vec4 p_color;

//Define any out variables for the fragment shader.
out vec3 FragPos;
out vec4 ParticleColor;

void main()
{
    gl_Position = MVP * vec4(vertex.x + offset.x, vertex.y + offset.y, vertex.z + offset.z, 1.0f);
	FragPos = vec3(model * vec4(vertex.x, vertex.y, vertex.z, 1.0f));
	ParticleColor = p_color;
}
