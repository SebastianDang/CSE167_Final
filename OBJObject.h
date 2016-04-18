#ifndef OBJOBJECT_H
#define OBJOBJECT_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

/* Vertex: [X, Y, Z] [R, G, B] [S, T] */
struct Vertex {
	// Position
	glm::vec3 Position;
	// Normal (Color)
	glm::vec3 Normal;
	// TexCoords
	glm::vec2 TexCoords;
};

struct Texture {
	GLuint id;
	std::string type;
};

class OBJObject
{
private:
	std::vector<glm::vec3> vertices;//v
	std::vector<glm::vec3> normals;//vn
	std::vector<unsigned int> indices;//f
	std::vector<Texture> textures;//List of textures
	glm::mat4 toWorld;

	GLuint VAO, VBO, EBO;
	void setupObject();

public:
	OBJObject(const char* filepath);
	~OBJObject();

	void parse(const char* filepath);
	void draw(GLuint shaderProgram);
	void scaleUp();
	void scaleDown();
	void reset();

	glm::vec3 trackBallMapping(glm::vec3 point);
	void rotate(glm::vec3 v, glm::vec3 w);
	void translate(glm::vec3 v, glm::vec3 w);
	void zoom(double y);

	void left();
	void right();
	void up();
	void down();
	void in();
	void out();
};

#endif