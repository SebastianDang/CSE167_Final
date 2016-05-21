#include "Bear.h"
#include "OBJObject.h"

/* Initialize the object, parse it and set up buffers. */
Bear::Bear(OBJObject * obj)
{
	this->toDraw = obj;
}

/* Deconstructor to safely delete when finished. */
Bear::~Bear()
{
}
