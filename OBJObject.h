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

	void spin(float);

public:
	OBJObject(const char* filepath);

	float angle;
	float orbitAngle;
	float pointSize;

	void parse(const char* filepath);
	void draw();
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