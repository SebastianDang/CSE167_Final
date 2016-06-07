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
	GLuint VAOBOX, VBOBOX;
	
	Material objMaterial;//Material
	int material;//Material selection

	//Parse the object to create it.
	void parse(const char* filepath);

	//Setup initial object materials, lighting.
	void setupObject();
	void setupMaterial();

	//Update object properties using these.
	void updateMaterial(GLuint shaderProgram);

	//Object movement.
	glm::vec3 currentDirection;
	float currentSpeed;
	float currentTurnSpeed;

public:
	/* Object constructor and setups */
	OBJObject(const char* filepath, int material);
	~OBJObject();

	//Keep track of world in relation to the object.
	glm::mat4 toWorld;

	//Draw.
	void draw(GLuint shaderProgram);

	//Object movement.
	void W_movement(glm::vec2 boundaries);
	void A_movement(glm::vec2 boundaries);
	void S_movement(glm::vec2 boundaries);
	void D_movement(glm::vec2 boundaries);
	void update_height(float height);

	//Object collision.
	std::vector<glm::vec3> boxCoords;

	bool collision(OBJObject * obj);
	void setupGeometry();
	void bindCube();
	void drawBox(GLuint shaderProgram);

	float minX, minY, minZ, maxX, maxY, maxZ, avgX, avgY, avgZ, scale_v;
	glm::vec3 average;

	float longestDim;

	int collisionFlag;

	float x_size, y_size, z_size;

};

#endif