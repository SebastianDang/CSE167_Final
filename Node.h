#pragma once
#ifndef NODE_H
#define NODE_H

#include "Window.h"

/* Class Node should be abstract and serve as the common base class. It should implement an abstract 
   draw method: virtual void draw() = 0, and also an abstract virtual void update(glm::mat4 C) = 0 method. */
class Node
{
private:
public:
	//Constructor methods.
	Node();
	~Node();
	//Abstract draw and upate methods.
	virtual void draw(GLuint shaderProgram) = 0;
	virtual void update(glm::mat4 C) = 0;
};
#endif