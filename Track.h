#pragma once

#include "window.h"
#include "MatrixTransform.h"
#include "Bezier.h"

class Track
{
private:
	std::vector<glm::vec3*> track_points;//Variable to hold trackpoints [24].
	std::vector<Bezier*> curves;//Variable to hold each curve [8].

	std::vector<glm::vec3*> control_points;//Variable to hold the control points (adjustments).
	std::vector<glm::vec3*> curve_points;//Variable to hold the curve points (actual points).

	void setupTrack();//Initial setup for the track

	/* Functions and variables for calculating pod location and orientation.*/
	float pod_position;
	float max_height_pos;
	float max_height_val;

	glm::vec3 getTrackPos(float t);
	void setMaxHeight();
	float calculateVelocity(float d_height);
	glm::vec4 updatePosition();
	glm::mat4 updateRotation();

public:
	//Constructors.
	Track();
	~Track();

	/* Functions for updating pod location and orientation.*/
	void resetPod();
	glm::mat4 updatePod();

	/* Draw. */
	void draw(GLuint shaderProgram);
	void drawPoints(GLuint shaderProgram);

	/* Adjustments on the points. */
	void movePoint(unsigned int track_point_id, glm::vec3 v, glm::vec3 w);
};
