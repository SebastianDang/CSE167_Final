#pragma once
#ifndef MATRIX_TRANSFORM_H
#define MATRIX_TRANSFORM_H

#include "Window.h"
#include "Group.h"

/* MatrixTransform should store a 4x4 transformation matrix M which is multiplied with matrix C, which is passed to the draw method. */
class MatrixTransform : public Group
{
private:
public:
	//Constructor methods.
	MatrixTransform();
	~MatrixTransform();
	//Stores a 4x4 transformation matrix in M.
	glm::mat4 M;
	//Upate methods.
	virtual void update(glm::mat4 C);
};
#endif
