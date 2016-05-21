#include "Track.h"
#include "OBJObject.h"
#include "Window.h"
#include "MatrixTransform.h"
#include "Bezier.h"

using namespace std;

#define NUM_POINTS 24
#define GRAVITY -0.000001f

/* Define a Default Track Constructor and initialize the Track Curve. */
Track::Track()
{
	//Setup initial values.
	this->pod_position = 0.0f;//3.289 if fmod == 8.
	this->max_height_pos = 0.0f;
	this->max_height_val = 0.0f;
	setupTrack();
}

/* Deconstructor to safely delete when finished. */
Track::~Track()
{
	for (int i = 0; i < curves.size(); i++)
	{
		delete(curves[i]);
	}
}

/* Setup the track to include all the points and curves necessary. */
void Track::setupTrack()
{
	/* Setup an arbitrary 24 points. */
	for (int i = 0; i < 360; i += 360 / NUM_POINTS)
	{
		float radians = (i / 180.0f * glm::pi<float>());//Convert to radians.
		glm::vec3 *t_point = new glm::vec3(10 * cos(radians), fmod(i, 8.0f), 10 * sin(radians));
		this->track_points.push_back(t_point);
	}
	track_points[24] = track_points[0];//Last track point has to point to the first since [0 - 24], 0 == 24.
	/* Setup the 8 curves associated with these points. */
	for (int i = 0; i < 8; i++)
	{
		Bezier * curve_n = new Bezier(track_points[3 * i], track_points[(3 * i) + 1], track_points[(3 * i) + 2], track_points[(3 * i) + 3]);
		this->curves.push_back(curve_n);
		this->curve_points.push_back(track_points[3 * i]);
		this->control_points.push_back(track_points[(3 * i) + 1]);
		this->control_points.push_back(track_points[(3 * i) + 2]);
		if (i > 0) {//Set up next and previous.
			Bezier * curve_set = curves[i];
			curve_set->prev = curves[i - 1];
			curve_set->prev->next = curve_set;
		}
	}
	Bezier * first = curves[0];//The last next and previous must be set.
	Bezier * last = curves[7];
	last->next = first;
	first->prev = last;
	/* Update the track to smooth out the initial edges.*/
	for (int i = 0; i < curves.size(); i++)
	{
		curves[i]->adjustCurvePrev();
		curves[i]->adjustCurveNext();
	}
	/* Set the pod to be at the max height. This also sets the max_height_position. */
	setMaxHeight();
	resetPod();
}

/* Gets the track position based on number of curves. By default we have 8 curves, so track position [0.0 - 8.0]*/
glm::vec3 Track::getTrackPos(float t)
{
	float time = (float)fmod(t, curves.size());//Sets t to be in the range of [0.00 - 7.99].
	int curve_num = (int)time;//Gets the current curve number [0 - 7].
	float curve_pos = time - (float)curve_num;//Gets the current position of specific curve [0.0 - 0.99]
	return(curves[curve_num]->getCurvePos(curve_pos));
}

/* Calculate the max height by looping through all the points. Sets max height value and position on the track. */
void Track::setMaxHeight()
{
	this->max_height_val = 0.0f;
	for (float i = 0.0f; i < (float)curves.size(); i = i += .01f)
	{
		glm::vec3 point = getTrackPos(i);
		if (point.y >= max_height_val)
		{
			this->max_height_val = point.y;
			this->max_height_pos = i;
		}
	}
}

/* Calculate the velocity at a given change in height. */
float Track::calculateVelocity(float d_height)
{
	float base_variable = (-2.0f) * GRAVITY;
	float base_height = d_height;//Delta h.
	if (base_height <= 0.0f)//Check just in case the variable passed through is negative or zero. (Also give it a nudge).
	{
		base_height = 0.01f;
	}
	float result = pow((base_variable * base_height), 0.5);
	return result;
}

/* Updates the position based on "Physics" velocity. */
glm::vec4 Track::updatePosition()
{
	//Get current position in terms of vec3.
	glm::vec3 position = getTrackPos(this->pod_position);
	//Calculate the velocity at that point using max height and velocity calculation.
	setMaxHeight();
	float velocity = calculateVelocity(this->max_height_val - position.y);
	//Update position change.
	this->pod_position += velocity;
	this->pod_position = fmod(this->pod_position, 8.0f);
	//return the next position.
	glm::vec3 updated_position = getTrackPos(this->pod_position);
	return glm::vec4(updated_position, 1.0f);
}

/* Automatically set the pod to be at the highest point of the track. */
void Track::resetPod()
{
	this->pod_position = this->max_height_pos;
}

/* Update the pod by combining new position and following orienation. */
glm::mat4 Track::updatePod()
{
	//Get the updated position.
	glm::vec4 update = updatePosition();
	//Create a vector from the new position.
	glm::vec3 pos = getTrackPos(this->pod_position);
	glm::vec3 target = getTrackPos(this->pod_position + 0.01f);
	//Calculating the resulting axes.
	glm::vec3 result_z = glm::normalize(glm::vec3(target) - pos);
	glm::vec3 result_x = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), result_z));
	glm::vec3 result_y = glm::normalize(glm::cross(result_z, result_x));
	target.y += 0.75f;//Orient it so it sits on the track (above it).
	//Construct the resulting matrix.
	glm::mat4 updated_pod = glm::mat4(glm::vec4(result_x, 0.0f), glm::vec4(result_y, 0.0f), glm::vec4(result_z, 0.0f), glm::vec4(target, 1.0f));
	return updated_pod;
}

/* Draw the Track curve. */
void Track::draw(GLuint shaderProgram)
{
	for (int i = 0; i < curves.size(); i++)
	{
		curves[i]->draw(shaderProgram);
	}
}

/* Draw the Track curve points with the curve id's. */
void Track::drawPoints(GLuint shaderProgram)
{
	for (int i = 0; i < curves.size(); i++)
	{
		curves[i]->draw(shaderProgram, 3*i);
	}
}

void Track::movePoint(unsigned int track_point_id, glm::vec3 v, glm::vec3 w)
{
	//Get the curve number and point at track_point_id.
	int curve_num = track_point_id / 3;
	if(curve_num >= curves.size())
	{
		curve_num = 0;
	}
	float point_num = fmod(track_point_id, 3.0f);
	//Calculate the translation (Screen -> World).
	glm::vec3 translate_v = glm::vec3(w.x - v.x, v.y - w.y, 0.0f);//Since x- and x+ are from left to right, y must be inverted so that y- and y+ are from bottom to top.
	glm::vec4 translate_v_4 = glm::vec4(translate_v, 1.0f);
	glm::mat4 P = Window::P;
	glm::mat4 V = Window::V;
	glm::mat4 P_inverse = glm::inverse(P);
	glm::mat4 V_inverse = glm::inverse(V);
	translate_v_4 = P_inverse * V_inverse * translate_v_4;
	translate_v = glm::vec3(translate_v_4);
	translate_v *= (float)(1 / (float)Window::width);//Scale the translation.
	//3 cases for the curve points.
	if (point_num == 0)
	{
		curves[curve_num]->adjustP0(translate_v);
	}
	else if (point_num == 1)
	{
		curves[curve_num]->adjustP1(translate_v);
	}
	else if (point_num == 2)
	{
		curves[curve_num]->adjustP2(translate_v);
	}
}