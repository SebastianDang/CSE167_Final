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
	int width;
	int height;

	std::vector<Terrain*> terrains;

public:
	//Constructor methods.
	Scenery(int width, int height);
	~Scenery();

	void generateTerrains();
	void stitchTerrains();

	void draw(GLuint shaderProgram);
};
#endif