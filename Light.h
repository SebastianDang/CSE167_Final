#pragma once

#include "Window.h"
#include "Definitions.h"

#ifndef LIGHT_H
#define LIGHT_H
class Light {
private:

	DirLight dirLight;
	PointLight pointLight;
	SpotLight spotLight;

	void setupLighting();
	void updateSelectLighting();

public:
	Light();
	~Light();

	int light_selection;

	void updateLighting(GLuint shaderProgram);
};
#endif