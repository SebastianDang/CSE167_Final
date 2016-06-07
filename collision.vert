#version 330 core

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 points;



//out DirLight fragDirLight;

uniform mat4 MVP;

void main()
{
    //fragDirLight = dirLight;
    gl_Position = MVP * vec4(points.x, points.y, points.z, 1.0);
}