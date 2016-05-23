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

	glm::mat4 toWorld;

	std::vector<Container> containers;//[v, vn, (s,t)]
	std::vector<glm::vec3> vertices;//v
	std::vector<glm::vec3> normals;//vn
	std::vector<glm::vec2> texCoords;//(s,t)

	std::vector<unsigned int> indices;//indices.

	GLuint VAO, VBO, EBO;

	void setupTerrain();
	void loadTerrain(const char* filename);

	unsigned char * loadPPM(const char* filename, int& width, int& height);

public:
	//Constructor methods.
	Terrain();
	~Terrain();

	//Draw and upate methods.
	void draw(GLuint shaderProgram);
	void update(glm::mat4 C);
};
#endif