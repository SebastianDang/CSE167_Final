#pragma once
#ifndef PARTICLE_H
#define PARTICLE_H

#include "Window.h"
#include "Definitions.h"
#include "OBJObject.h"

class Particle
{
private:

	std::vector<Particles_struct> particles;
	std::vector<glm::vec3> vertices;//v
	std::vector<unsigned int> indices;//f

	float x, z;

	GLuint VAO, VBO, EBO;

	glm::mat4 toWorld;
	float gravity;
	OBJObject * toFollow;

	void setupGeometry();
	void setupParticle();

	GLuint FirstUnusedParticle();
	void RespawnParticle(Particles_struct &particle);
	void animate(Particles_struct &particle);
public:
	/* Object constructor and setups */
	Particle(int x_d, int z_d);
	Particle(OBJObject * follow);
	~Particle();

	void update();
	void draw(GLuint shaderProgram);

	void increaseGravity();
	void decreaseGravity();
};

#endif