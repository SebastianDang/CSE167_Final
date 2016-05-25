#pragma once
#ifndef TERRAIN_H
#define TERRAIN_H

#include "Window.h"
#include "OBJObject.h"

class Terrain
{
private:
	float x;
	float z;

	GLenum draw_mode;

	glm::mat4 toWorld;

	GLuint terrainTexture;

	std::vector<Container> containers;//[v, vn, (s,t)]
	std::vector<glm::vec3> vertices;//v
	std::vector<glm::vec3> normals;//vn
	std::vector<glm::vec2> texCoords;//(s,t)

	std::vector<unsigned int> indices;//indices.

	GLuint VAO, VBO, EBO;

	void setupHeightMap();
	void setupHeightMap(const char* filename);

	float getHeightFromMap(int x, int y, unsigned char * image, int width, int height);
	glm::vec3 calculateNormal(int x, int y, unsigned char * image, int width, int height);
	void diamond_square(float x1, float x2, float y1, float y2);


	void setupTerrain(const char* filename);
	GLuint loadTerrain(const char* filename);

	unsigned char * loadPPM(const char* filename, int& width, int& height);

public:
	//Constructor methods.
	Terrain();
	~Terrain();

	//Draw and upate methods.
	void draw(GLuint shaderProgram);
	void update(glm::mat4 C);
	void toggleDrawMode();
};
#endif