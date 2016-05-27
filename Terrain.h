#pragma once
#ifndef TERRAIN_H
#define TERRAIN_H

#include "Window.h"
#include "OBJObject.h"

class Terrain
{
private:
	//Determine the terrain's position in the world.
	float x;
	float z;
	glm::mat4 toWorld;
	//Determine the textures mapped to the terrain, using a blend map.
	GLuint terrainTexture_0;
	GLuint terrainTexture_1;
	GLuint terrainTexture_2;
	GLuint terrainTexture_3;
	GLuint blendMap;
	//Keep track of the max and min height if height map is loaded.
	float max_height;
	float min_height;
	//Variables to keep track of information.
	std::vector<Container> containers;//[v, vn, (s,t)]
	std::vector<glm::vec3> vertices;//v
	std::vector<glm::vec3> normals;//vn
	std::vector<glm::vec2> texCoords;//(s,t)
	std::vector<unsigned int> indices;//indices.
	GLuint VAO, VBO, EBO;
	//Flat terrain map.
	void setupHeightMap();
	//Functions for procedural terrain modeling.
	void setupHeightMap(const char* filename, float n_smooth, float n_range);
	float getHeightFromMap(int x, int y, unsigned char * image, int width, int height);
	void diamond_square(int x1, int x2, int y1, int y2, int level, float range);
	void updateNormals();
	void updateMaxMinHeight();
	//Load and setup the textures and heightmaps.
	unsigned char * loadPPM(const char* filename, int& width, int& height);
	GLuint loadTerrain(const char* filename, int index);
	void setupTerrain(const char* terrain_0, const char* terrain_1, const char* terrain_2, const char* terrain_3, const char* blend_map);
	//Misc.
	int draw_mode;

public:
	//Constructor methods.
	Terrain(float x_d, float z_d, const char* terrain_0, const char* terrain_1, const char* terrain_2, const char* terrain_3, const char* blend_map);
	Terrain(float x_d, float z_d, const char* terrain_0, const char* terrain_1, const char* terrain_2, const char* terrain_3, const char* blend_map, const char* height_map);
	~Terrain();

	//Draw and upate methods.
	void toggleDrawMode();
	void draw(GLuint shaderProgram);
};
#endif