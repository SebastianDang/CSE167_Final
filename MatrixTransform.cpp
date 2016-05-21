#include "MatrixTransform.h"

/* Define a MatrixTransform Constructor and initialize the Matrix M. */
MatrixTransform::MatrixTransform() 
{
	M = glm::mat4(1.0f);
}

/* Update the transformation matrix by multiplying the current Matrix M by C. */
void MatrixTransform::update(glm::mat4 C)
{
	glm::mat4 result = C*M;
	Group::update(result);
}

