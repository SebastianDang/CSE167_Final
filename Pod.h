#ifndef POD_H
#define POD_H

#include "Window.h"
#include "Geode.h"

class Pod : public Geode
{
private:
public:
	/* Object constructor and setups */
	Pod(OBJObject * obj);
	~Pod();
};

#endif