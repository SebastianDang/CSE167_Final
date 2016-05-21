#pragma once
#ifndef GEODE_H
#define GEODE_H

#include "Window.h"
#include "Node.h"


/* Geode should be an abstract class. It should update the rendering matrix, and have a draw function to draw its geometry. */
class Geode : public Node
{
private:
public:
	//Constructor methods.
	Geode();
	Geode(OBJObject * obj);
	~Geode();
	//The object to be drawn and matrix M.
	OBJObject *toDraw;
	glm::mat4 M;
	//Draw and upate methods.
	virtual void draw(GLuint shaderProgram);
	virtual void update(glm::mat4 C);
};
#endif