#ifndef OBJOBJECT_H
#define OBJOBJECT_H

#include "Window.h"
#include "Definitions.h"

class OBJObject
{
private:
	std::vector<Container> containers;//[v, vn, (s,t)]
	std::vector<glm::vec3> vertices;//v
	std::vector<glm::vec3> normals;//vn
	std::vector<unsigned int> indices;//f
	std::vector<Texture> textures;//List of textures

	GLuint VAO, VBO, EBO;
	
	Material objMaterial;//Material
	int material;//Material selection

	//Parse the object to create it.
	void parse(const char* filepath);

	//Setup initial object materials, lighting.
	void setupObject();
	void setupMaterial();

	//Update object properties using these.
	void updateMaterial(GLuint shaderProgram);

public:
	/* Object constructor and setups */
	OBJObject(const char* filepath, int material);
	~OBJObject();

	//Keep track of world in relation to the object.
	glm::mat4 toWorld;

	void draw(GLuint shaderProgram);
};

#endif