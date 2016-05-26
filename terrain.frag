#version 330 core

uniform sampler2D TerrainTexture_0;
uniform sampler2D TerrainTexture_1;
uniform sampler2D TerrainTexture_2;
uniform sampler2D TerrainTexture_3;

uniform float max_height;
uniform float min_height;

//Define any in variables from the vertex shader.
in vec3 FragPos;
in vec3 FragNormal;
in vec2 FragTexCoords;

//Define out variable for the fragment shader: color.
out vec4 color;

void main()
{
	float d_height = (max_height - min_height)/100;
	vec4 total_color = vec4(0.0, 0.0, 0.0, 0.0);

	if (FragPos.y < (min_height + 30*d_height))
	{
		total_color += texture(TerrainTexture_0, FragTexCoords);
	}
	if (FragPos.y > (min_height + 29*d_height) && FragPos.y < (min_height + 60*d_height))
	{
		total_color += texture(TerrainTexture_1, FragTexCoords);
	}
	if (FragPos.y > (min_height + 59*d_height))
	{
		total_color += texture(TerrainTexture_2, FragTexCoords);
	}
	
	color = total_color;
}

