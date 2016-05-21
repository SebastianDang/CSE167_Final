#version 330 core

//The vertex shader gets called once per vertex.

//Define the position defined in the SkyBox.
layout (location = 0) in vec3 vertex;

//Define uniform MVP: model, view, projection passed from the object.
uniform mat4 MVP;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//Define any out variables for the fragment shader.
out vec3 TexCoords;

void main()
{
    gl_Position =  projection * view * vec4(vertex.x, vertex.y, vertex.z, 1.0f);  
    TexCoords = vertex;
}  