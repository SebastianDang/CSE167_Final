#pragma once
#pragma once
#ifndef SCENERY_H
#define SCENERY_H

#include "Window.h"
#include "Definitions.h"
#include "Terrain.h"

class Scenery
{
private:
	//Contain the width and height of the scenery.
	int width;
	int height;
	//Access terrains from the scenery class.
	std::vector<Terrain*> terrains;
	void generateTerrains();
	void stitchTerrains();
	int getTerrain(glm::vec3 position);

public:
	//Constructor methods.
	Scenery(int width, int height);
	~Scenery();

	float getHeight(glm::vec3 position);

	void toggleDrawMode();

	void draw(GLuint shaderProgram);
};
#endif