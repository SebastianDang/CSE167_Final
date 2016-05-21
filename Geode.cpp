#include "Geode.h"

/* Define a Geode Constructor amd initialize the Geode. */
Geode::Geode()
{
}

Geode::Geode(OBJObject * obj)
{
	this->toDraw = obj;
}

/* Deconstructor to safely delete when finished. */
Geode::~Geode()
{
}

/* Call draw on the specific ObjObject. */
void Geode::draw(GLuint shaderProgram)
{
	toDraw->toWorld = this->M;
	toDraw->draw(shaderProgram);
}

/* Update the current Matrix with C.*/
void Geode::update(glm::mat4 C)
{
	this->M = C;
}