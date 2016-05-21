#ifndef BEAR_H
#define BEAR_H

#include "Window.h"
#include "Geode.h"

class Bear : public Geode
{
private:
public:
	/* Object constructor and setups */
	Bear(OBJObject * obj);
	~Bear();
};
#endif