#ifndef SkyBox_H
#define SkyBox_H

#include "Window.h"

class SkyBox
{
private:
	std::vector<glm::vec3> vertices;//v
	std::vector<unsigned int> indices;//f
	std::vector<const GLchar*> faces;//Inside face walls.

	GLuint VAO, VBO, EBO;

	//Keep track of world in relation to the object.
	glm::mat4 toWorld;
	GLuint cubemapTexture;

	unsigned char* loadPPM(const char* filename, int& width, int& height);

	void setupCube();
	void setupFaces();
	void setupSkyBox();

	GLuint loadCubemap(std::vector<const GLchar*> faces);

public:
	SkyBox();
	~SkyBox();

	void draw(GLuint);
	GLuint getSkyBox();
};

#endif

