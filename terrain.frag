#version 330 core

uniform sampler2D TerrainTexture;

//Define any in variables from the vertex shader.
in vec3 FragPos;
in vec3 FragNormal;
in vec2 FragTexCoords;

//Define out variable for the fragment shader: color.
out vec4 color;

void main()
{
	color = texture(TerrainTexture, FragTexCoords);

}

