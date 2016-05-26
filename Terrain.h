#pragma once
#ifndef TERRAIN_H
#define TERRAIN_H

#include "Window.h"
#include "OBJObject.h"

class Terrain
{
private:
	int draw_mode;

	glm::mat4 toWorld;

	GLuint terrainTexture;

	std::vector<Container> containers;//[v, vn, (s,t)]
	std::vector<glm::vec3> vertices;//v
	std::vector<glm::vec3> normals;//vn
	std::vector<glm::vec2> texCoords;//(s,t)

	std::vector<unsigned int> indices;//indices.

	GLuint VAO, VBO, EBO;

	void setupHeightMap();
	void setupHeightMap(const char* filename, float n_smooth, float n_range);

	float getHeightFromMap(int x, int y, unsigned char * image, int width, int height);
	void diamond_square(int x1, int x2, int y1, int y2, int level, float range);
	void updateNormals();

	unsigned char * loadPPM(const char* filename, int& width, int& height);
	GLuint loadTerrain(const char* filename);
	void setupTerrain(const char* filename);

public:
	//Constructor methods.
	Terrain();
	~Terrain();

	//Draw and upate methods.
	void toggleDrawMode();
	void draw(GLuint shaderProgram);
};
#endif