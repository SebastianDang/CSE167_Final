#include "Pod.h"
#include "OBJObject.h"

/* Initialize the object, parse it and set up buffers. */
Pod::Pod(OBJObject * obj)
{
	this->toDraw = obj;
}

/* Deconstructor to safely delete when finished. */
Pod::~Pod()
{

}

