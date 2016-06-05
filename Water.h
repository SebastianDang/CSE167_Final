//
//  Water.h
//  Project1
//
//  Created by Parth Patel on 5/27/16.
//  Copyright © 2016 Parth Patel. All rights reserved.
//

#ifndef Water_h
#define Water_h

#include "Window.h"

//A struct to hold a control point of the surface.
struct Point {
	float x;
	float y;
	float z;
};

class Water
{
private:
	//Water properties.
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<int> indices;
	unsigned int level_of_detail;
	//GLSL properties.
	int draw_mode;
	glm::mat4 toWorld;
	GLuint VBO, VAO, EBO, VBONORM;
	//Intialization functions.
	void setupGeometry();
	Point CalculateU(float t, int row);
	Point CalculateV(float t, Point* pnts);
	Point Calculate(float u, float v);
	void setupWater();

public:
	Water();
	Water(GLuint skyBox_texture);
    ~Water();

	GLuint skyTexture;

	void toggleDrawMode();
	void draw(GLuint);
};

#endif
