#ifndef CYLINDER_H
#define CYLINDER_H

#include "Window.h"
#include "Geode.h"

class Cylinder : public Geode
{
private:
public:
	/* Object constructor and setups */
	Cylinder::Cylinder(OBJObject * obj);
	~Cylinder();

};

#endif