#include "Cylinder.h"
#include "OBJObject.h"

/* Initialize the object, parse it and set up buffers. */
Cylinder::Cylinder(OBJObject * obj)
{
	this->toDraw = obj;
}

/* Deconstructor to safely delete when finished. */
Cylinder::~Cylinder()
{
}