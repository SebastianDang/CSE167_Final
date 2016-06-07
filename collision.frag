#version 330 core

// This is a sample fragment shader.
out vec4 color;
uniform int collisionFlag;

void main()
{
    if(collisionFlag == 0)
        color = vec4(0.0f,0.0f,1.0f, 0.0f);
    if(collisionFlag == 1)
        color = vec4(1.0f,0.0f,0.0f, 0.0f);
    
}
