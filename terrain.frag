#version 330 core

uniform sampler2D TerrainTexture_0;
uniform sampler2D TerrainTexture_1;
uniform sampler2D TerrainTexture_2;
uniform sampler2D TerrainTexture_3;
uniform sampler2D blendMap;

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
	vec4 total_color = vec4(0.0, 0.0, 0.0, 0.0);

	vec4 blendMapColor = texture(blendMap, FragTexCoords);

	float TerrainTexture_0_amount = 1 - (blendMapColor.r + blendMapColor.g + blendMapColor.b);
	vec2 tiledCoords = FragTexCoords * 40.0;

	vec4 TerrainTexture_0_color = texture(TerrainTexture_0, tiledCoords) * TerrainTexture_0_amount;
	
	vec4 TerrainTexture_1_color = texture(TerrainTexture_1, tiledCoords) * blendMapColor.r;
	vec4 TerrainTexture_2_color = texture(TerrainTexture_2, tiledCoords) * blendMapColor.g;
	vec4 TerrainTexture_3_color = texture(TerrainTexture_3, tiledCoords) * blendMapColor.b;
	 
	total_color += TerrainTexture_0_color + TerrainTexture_1_color + TerrainTexture_2_color + TerrainTexture_3_color;
	color = total_color;
	//color = texture(blendMap, FragTexCoords);

}

