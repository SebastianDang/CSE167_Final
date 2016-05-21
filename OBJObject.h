#ifndef OBJOBJECT_H
#define OBJOBJECT_H

#include "Window.h"

/* Container: [X, Y, Z] [R, G, B] [S, T] */
struct Container {
	//Position
	glm::vec3 vertex;
	//Normal
	glm::vec3 normal;
	//TexCoords
	glm::vec2 texCoord;
};

/* Texture Container to hold certain textures. */
struct Texture {
	GLuint id;
	std::string type;
};

struct Material {//Setup the object's material.
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

struct DirLight {//Directional Light
	bool on;
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

struct PointLight {//Point Light
	bool on;
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float quadratic;
};

struct SpotLight {//Spot Light
	bool on;
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float quadratic;
	glm::vec3 direction;
	float spotCutoff;
	float spotExponent;
};

struct Camera {//Global Camera.
	glm::vec3 position;
	glm::vec3 lookat;
	glm::vec3 up;
	glm::vec3 direction;
	glm::vec3 right;
};

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

	DirLight dirLight;
	PointLight pointLight;
	SpotLight spotLight;
	Camera camera;

	//Parse the object to create it.
	void parse(const char* filepath);

	//Setup initial object materials, lighting, and camera.
	void setupObject();
	void setupMaterial();
	void setupLighting();
	void setupCamera();

	//Update object properties using these.
	void updateMaterial(GLuint shaderProgram);
	void updateSelectLighting();
	void updateLighting(GLuint shaderProgram);
	void updateCamera(glm::vec3 e, glm::vec3 d, glm::vec3 up);

public:
	/* Object constructor and setups */
	OBJObject(const char* filepath, int material);
	~OBJObject();

	//Keep track of world in relation to the object.
	glm::mat4 toWorld;

	int light_selection;

	/* Object set up methods */
	void draw(GLuint shaderProgram);
	void window_updateCamera();
	glm::vec3 trackBallMapping(glm::vec3 point);

	/* Mouse Control */
	void camera_rotate(glm::vec3 v, glm::vec3 w);
	void translate(glm::vec3 v, glm::vec3 w);
	void zoom(double y);
};

#endif