#pragma once

#include "window.h"
#include "Geode.h"

class Bezier
{
private:
	//Hold the curve in a matrix [p0, p1, p2, p3].
	glm::mat4x3 curve;

	//Hold 100 points for the segments of the curve (points).
	std::vector<glm::vec3> segments;
	std::vector<glm::vec3> points;

	//Other variables.
	GLuint VAO, VBO, VAO_P, VBO_P;
	glm::mat4 toWorld;

	//Functions to initialize the curve.
	void setupSegments();
	void setupPoints();
	void setupBezier();
	
	//Functions to calculate points along the curve.
	float choose(float i);
	glm::vec4 computeConstant(float t);
	glm::vec3 computeCurve(float t);
	void calculatePoints();

public:
	//Constructors.
	Bezier();
	Bezier(glm::vec3 *p0, glm::vec3 *p1, glm::vec3 *p2, glm::vec3 *p3);
	~Bezier();
	
	//Hold the points of the current curve.
	glm::vec3 *p0;
	glm::vec3 *p1;
	glm::vec3 *p2;
	glm::vec3 *p3;

	//Prev and Next to calculate C1 continuity.
	Bezier *prev;
	Bezier *next;

	//Adjustments for C1 continuity.
	void adjustCurvePrev();
	void adjustCurveNext();

	//Adjust 4 points.
	void adjustP0(glm::vec3 translate);
	void adjustP1(glm::vec3 translate);
	void adjustP2(glm::vec3 translate);

	//Functions.
	void updateCurve();
	glm::vec3 getCurvePos(float t);
	void draw(GLuint shaderProgram);
	void draw(GLuint shaderProgram, float id);
};
