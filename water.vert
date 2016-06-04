#version 330 core

//The vertex shader gets called once per vertex.

//Define position, normal, and texture defined in the Container.
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;

//Define uniform MVP: model, view, projection passed from the object.
uniform mat4 MVP;
uniform mat4 model;
uniform float time; //elasped time

out vec3 FragNormal;
out vec3 FragPos;

//shader constants
const float amplitude = 0.625;//.525
const float frequency = 20;//5
const float PI = 3.14159;


void main()
{
    //Calculate the Ripple Effect
    float dist_vertex = length(vertex);//Get the Euclidean distance of the current vertex from the center of the mesh
    float y_vertex = amplitude*sin((-PI * dist_vertex * frequency) + time) + amplitude*cos((-PI * dist_vertex * frequency) + time);//create a sin function using the distance, multiply frequency and add the elapsed time

    //multiply the MVP matrix with the new position to get the clipspace position
    gl_Position = MVP * vec4(vertex.x, y_vertex, vertex.z, 1.0);
   
	FragNormal = vec3( mat4(transpose(inverse(model)))  * vec4(normal.x, normal.y, normal.z, 1.0f));
    FragPos = vec3(model * vec4(vertex.x, y_vertex, vertex.z, 1.0f));
    

}