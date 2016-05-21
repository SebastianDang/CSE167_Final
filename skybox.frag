#version 330 core

//Define any in variables from the vertex shader.
in vec3 TexCoords;

uniform samplerCube skybox;

//Define color for output to screen.
out vec4 color;

void main()
{    
    color = texture(skybox, TexCoords);
}
  