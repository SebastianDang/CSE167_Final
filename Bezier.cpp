#include "Bezier.h"
#include "OBJObject.h"
#include "Window.h"

using namespace std;

#define LINE_WIDTH 2.0f
#define POINT_SIZE 7.0f

/* Define a Default Bezier Constructor and initialize the Bezier Curve. */
Bezier::Bezier()
{
	glm::vec3 *point1 = new glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 *point2 = new glm::vec3(3.5f, 0.0f, 0.0f);
	glm::vec3 *point3 = new glm::vec3(7.0f, 0.0f, 0.0f);
	glm::vec3 *point4 = new glm::vec3(10.0f, 0.0f, 0.0f);
	Bezier(point1, point2, point3, point4);
}

/* Define a Bezier Constructor that takes in 4 points and sets them to each control point. */
Bezier::Bezier(glm::vec3 *p0, glm::vec3 *p1, glm::vec3 *p2, glm::vec3 *p3)
{
	//Default toWorld (Center).
	this->toWorld = glm::mat4(1.0f);
	//Set control point variables to the 4 coordinates.
	this->p0 = p0;//POINT
	this->p1 = p1;//HANDLE
	this->p2 = p2;//HANDLE
	this->p3 = p3;//POINT
	//Setup the vectors so they contain the proper data.
	setupSegments();
	setupPoints();
	//Update the curve given the 4 points.
	updateCurve();
	//Setup the Bezier.
	setupBezier();
}

/* Deconstructor to safely delete when finished. */
Bezier::~Bezier()
{
	glDeleteVertexArrays(1, &this->VAO);
	glDeleteBuffers(1, &this->VBO);
	delete(this->p0);
	delete(this->p1);
	delete(this->p2);
}

/* Setup the vector to allow a size of 100 elements. */
void Bezier::setupSegments()
{
	for (int i = 0; i <= 100; i++)
	{
		this->segments.push_back(glm::vec3(0.0f));
	}
}

/* Setup the points so we can use them for rendering later. */
void Bezier::setupPoints()
{
	points.push_back(glm::vec3(this->p0->x, this->p0->y, this->p0->z));
	points.push_back(glm::vec3(this->p1->x, this->p1->y, this->p1->z));
	points.push_back(glm::vec3(this->p2->x, this->p2->y, this->p2->z));
	points.push_back(glm::vec3(this->p3->x, this->p3->y, this->p3->z));
}

/* Setup the object for modern openGL rendering. Setup all VAO's, VBO's. */
void Bezier::setupBezier()
{
	/* Segments */
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);//Bind vertex array object.

	glBindBuffer(GL_ARRAY_BUFFER, VBO);//Bind buffer for segments.
	glBufferData(GL_ARRAY_BUFFER, this->segments.size() * 3 * sizeof(GLfloat), &this->segments[0], GL_DYNAMIC_DRAW);//Set vertex buffer to the segments data.

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	/* Points */
	glGenVertexArrays(1, &VAO_P);
	glGenBuffers(1, &VBO_P);

	glBindVertexArray(this->VAO_P); //Bind vertex array object.

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO_P);//Bind buffer for points.
	glBufferData(GL_ARRAY_BUFFER, this->points.size() * 3 * sizeof(GLfloat), &this->points[0], GL_DYNAMIC_DRAW); //Set vertex buffer to the points data.

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
}

/* [1] Compute the choose section. Here let's assume a 3 choose i */
float Bezier::choose(float i)
{//Since we precompute these, we know it always returns 1, 3, 3, 1 for 0, 1, 2, 3 respectively.
	if (i == 0)
	{//(3!)/[(3!)(0!)] = 1.
		return 1;
	}
	else if (i == 1)
	{//(3!)/[(2!)(1!)] = 3.
		return 3;
	}
	else if (i == 2)
	{//(3!)/[(1!)(2!)] = 3.
		return 3;
	}
	else if (i == 3)
	{//(3!)/[(0!)(3!)] = 1.
		return 1;
	}
}

/* [1] Compute the constant C for the Bezier curve. The variable 't' stays within the range [0, 1]. Returns a vec4 with each constant. */
glm::vec4 Bezier::computeConstant(float t)
{
	/* Compute with a for-loop. */
	float n = 3;
	glm::vec4 result = glm::vec4(0.0f);
	for (int i = 0; i < (n+1); i++)
	{
		float choose = (float)this->choose((float)i);
		float time1 = (float)pow(t, i);
		float time2 = (float)pow(1.0f - t, n - i);
		float constant = choose*time1*time2;
		result[i] = constant;
	}
	/* Return the computed constant. */
	return result;
}

/* [1] Compute the calculated curve for given 't' and points p0, p1, p2, p3. Return a vec3 with proper position. */
glm::vec3 Bezier::computeCurve(float t)
{
	glm::vec4 constant = computeConstant(t);//Calculate the constant.
	glm::vec3 result = this->curve * constant;//Multiply the constant to the 4 positions to get the point at time 't'.
	/* Return the computed curve. */
	return result;
}

/* [100] Calculate each point with computeCurve and add it into the segments vector in the proper location. */
void Bezier::calculatePoints()
{
	for (int i = 0; i <= 100; i++)
	{
		float interval = i * (.01f);
		glm::vec3 point = computeCurve(interval);
		this->segments[i] = point;
	}
}

/* Returns the position at time t, along the curve. */
glm::vec3 Bezier::getCurvePos(float t)
{
	int curve_pos = (int)(t*100.0f);//convert to array position.
	return segments[curve_pos];//return points array position.
}

/* Adjust the previous curve to match this curve. If we move p1! */
void Bezier::adjustCurvePrev()
{
	glm::vec3 tangent = glm::vec3(this->p1->x - this->p0->x, this->p1->y - this->p0->y, this->p1->z - this->p0->z);
	glm::vec3 *adjustment = this->prev->p3;
	this->prev->p2->x = adjustment->x - tangent.x;
	this->prev->p2->y = adjustment->y - tangent.y;
	this->prev->p2->z = adjustment->z - tangent.z;
	this->prev->updateCurve();
}

/* Adjust the next curve to match this curve. If we move p2! */
void Bezier::adjustCurveNext()
{
	glm::vec3 tangent = glm::vec3(this->p2->x - this->p3->x, this->p2->y - this->p3->y, this->p2->z - this->p3->z);
	glm::vec3 *adjustment = this->next->p0;
	this->next->p1->x = adjustment->x - tangent.x;
	this->next->p1->y = adjustment->y - tangent.y;
	this->next->p1->z = adjustment->z - tangent.z;
	this->next->updateCurve();
}

/* [1] Update the curve by calculating each point and updating their locations based on the 4 control points. Depends on whether the points are updated or not. */
void Bezier::updateCurve()
{
	//Update the set of points.
	this->points[0] = *this->p0;
	this->points[1] = *this->p1;
	this->points[2] = *this->p2;
	this->points[3] = *this->p3;
	//Update the curve.
	this->curve = glm::mat4x3(*this->p0, *this->p1, *this->p2, *this->p3);//Set it to a matrix for convenience.
	calculatePoints();//Calculate points along the curve
	//Update the segments buffer to redraw.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, this->segments.size() * 3 * sizeof(GLfloat), &this->segments[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Update the points buffer to redraw.
	glBindBuffer(GL_ARRAY_BUFFER, VBO_P);
	glBufferSubData(GL_ARRAY_BUFFER, 0, this->points.size() * 3 * sizeof(GLfloat), &this->points[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/* [0-100] Responsible for drawing the bezier curve. This loops through. */
void Bezier::draw(GLuint shaderProgram)
{
	//Calculate combination of the model (toWorld), view (camera inverse), and perspective matrices. Send to shader.
	glm::mat4 MVP = Window::P * Window::V * toWorld;
	//Set MVP(Total calculated, easier to multiply in the shader) for the shader.
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	//Set the line width.
	glLineWidth(LINE_WIDTH);
	//Set color.
	glUniform3f(glGetUniformLocation(shaderProgram, "set_color"), 0.82f, 0.8f, 0.0f);
	//Bind for rendering.
	glBindVertexArray(this->VAO);
	for (int i = 0; i < segments.size()-1; i++) {
		glDrawArrays(GL_LINES, i, 2);
	}
	glBindVertexArray(0);
	//Set the point size and width.
	glPointSize(POINT_SIZE);
	glLineWidth(LINE_WIDTH * 0.6f);
	//Set color for the end points.
	glUniform3f(glGetUniformLocation(shaderProgram, "set_color"), 1.0f, 0.0f, 0.0f);
	//Bind for rendering.
	glBindVertexArray(this->VAO_P);
	glDrawArrays(GL_POINTS, 0, 1);//Draw first point.
	//glDrawArrays(GL_POINTS, 3, 1);
	//Set color for the middle control points.
	glUniform3f(glGetUniformLocation(shaderProgram, "set_color"), 0.0f, 1.0f, 0.0f);
	glDrawArrays(GL_POINTS, 1, 2);
	//Draw lines between to find tangents.
	glUniform3f(glGetUniformLocation(shaderProgram, "set_color"), 0.2f, 0.8f, 0.0f);
	glDrawArrays(GL_LINES, 0, 2);
	glDrawArrays(GL_LINES, 2, 2);
	glBindVertexArray(0);
	
}

/* [1-24] Draw the points for the selection shader. */
void Bezier::draw(GLuint shaderProgram, float id)
{
	//Calculate combination of the model (toWorld), view (camera inverse), and perspective matrices. Send to shader.
	glm::mat4 MVP = Window::P * Window::V * toWorld;
	//Set MVP(Total calculated, easier to multiply in the shader) for the shader.
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	//Set the point size and width.
	glPointSize(2*POINT_SIZE);
	//Bind for rendering.
	glBindVertexArray(this->VAO_P);
	//glUniform3f(glGetUniformLocation(shaderProgram, "set_color"), 1.0f, 0.0f, id / 255.0f);
	//glDrawArrays(GL_POINTS, 0, 1);//Draw first point.
	glUniform3f(glGetUniformLocation(shaderProgram, "id"), 0.0f, 1.0f, (id + 1)/255.0f);
	glDrawArrays(GL_POINTS, 1, 1);//Draw second point.
	glUniform3f(glGetUniformLocation(shaderProgram, "id"), 0.0f, 1.0f, (id + 2) / 255.0f);
	glDrawArrays(GL_POINTS, 2, 1);//Draw third point.
	glUniform3f(glGetUniformLocation(shaderProgram, "id"), 1.0f, 0.0f, (id + 3) / 255.0f);
	glDrawArrays(GL_POINTS, 3, 1);
	glBindVertexArray(0);
}

/* Adjust p0 when selected. */
void Bezier::adjustP0(glm::vec3 translate)
{
	//Adjust p0.
	this->p0->x += translate.x;
	this->p0->y += translate.y;
	this->p0->z += translate.z;
	//Translate control point associated with it.
	this->p1->x += translate.x;
	this->p1->y += translate.y;
	this->p1->z += translate.z;
	//Update the opposite control point.
	adjustCurvePrev();
	//Update the current curve.
	updateCurve();
}

/* Adjust p1 when selected. */
void Bezier::adjustP1(glm::vec3 translate)
{
	//Adjust p1.
	this->p1->x += translate.x;
	this->p1->y += translate.y;
	this->p1->z += translate.z;
	//Update the opposite control point.
	adjustCurvePrev();
	//Update the current curve.
	updateCurve();
}

/* Adjust p2 when selected. */
void Bezier::adjustP2(glm::vec3 translate)
{
	//Adjust p2.
	this->p2->x += translate.x;
	this->p2->y += translate.y;
	this->p2->z += translate.z;
	//Update the opposite control point.
	adjustCurveNext();
	//Update the current curve.
	updateCurve();
}