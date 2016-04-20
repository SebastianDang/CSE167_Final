#ifndef OBJOBJECT_H
#define OBJOBJECT_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

/* Container: [X, Y, Z] [R, G, B] [S, T] */
struct Container {
	// Position
	glm::vec3 vertex;
	// Normal (Color)
	glm::vec3 normal;
	// TexCoords
	glm::vec2 texCoord;
};

struct Texture {
	GLuint id;
	std::string type;
};

class OBJObject
{
private:
	std::vector<Container> containers;
	std::vector<glm::vec3> vertices;//v
	std::vector<glm::vec3> normals;//vn
	std::vector<unsigned int> indices;//f
	std::vector<Texture> textures;//List of textures
	
	glm::mat4 toWorld;

	GLuint VAO, VBO, EBO;
	void setupObject();
	void setupLights();

public:
	OBJObject(const char* filepath);
	~OBJObject();

	int material;
	int light_selection;

	void parse(const char* filepath);
	void draw(GLuint shaderProgram);
	void setupMaterial(GLuint shaderProgram, int selection);
	void setupLighting(GLuint shaderProgram);
	void selectLighting(GLuint shaderProgram, int selection);
	void scaleUp();
	void scaleDown();
	void reset();
	glm::vec3 trackBallMapping(glm::vec3 point);
	void rotate(glm::vec3 v, glm::vec3 w);
	void translate(glm::vec3 v, glm::vec3 w);
	void zoom(double y);

};

#endif