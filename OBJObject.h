#ifndef OBJOBJECT_H
#define OBJOBJECT_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class OBJObject
{
private:
	std::vector<unsigned int> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	glm::mat4 toWorld;
	glm::mat4 c_inverse;
	glm::mat4 projection;
	glm::mat4 viewport;

public:
	OBJObject(const char* filepath);

	std::vector<unsigned int> getIndices();
	std::vector<glm::vec3> getVertices();
	std::vector<glm::vec3> getNormals();
	glm::mat4 getWorld();
	glm::mat4 getCamera();
	glm::mat4 getProjection();
	glm::mat4 getViewport();

	void setWorld();
	void setCamera();
	void setProjection(float, float);
	void setViewport(float, float);
	
	float angle;
	float orbitAngle;
	float pointSize;
	float getPointSize();

	void parse(const char* filepath);
	void draw();
	void draw(GLuint shaderProgram);
	void spin(float);
	void update();
	void pointUp();
	void pointDown();
	void left();
	void right();
	void up();
	void down();
	void in();
	void out();
	void scaleUp();
	void scaleDown();
	void orbitCW();
	void orbitCCW();
	void reset();
};

#endif