#include "Particle.h"
#include "Definitions.h"
#include <time.h>

#define SIZE 40.0f
#define AREA_SIZE 500
#define RANDOM_SIZE 500
#define RANDOM_HEIGHT 0.5f
#define WATER_HEIGHT (SIZE + 3)

GLuint nr_particles = 1000;
GLuint lastUsedParticle = 0;

/* Default constructor for a particle with no shape or direction. */
Particle::Particle(int x_d, int z_d)
{
	//Setup particle properties.
	this->x = x_d * AREA_SIZE + (AREA_SIZE/2);
	this->z = z_d * AREA_SIZE + (AREA_SIZE/2);
	this->toWorld = glm::mat4(1.0f);
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(this->x, 0, this->z));
	this->toWorld = translate*this->toWorld;

	//Setup the geometry of the particle and generate the VAO.
	setupGeometry();
	setupParticle();

	//Setup the particles generator.
	for (GLuint i = 0; i < nr_particles; ++i) 
	{
		this->particles.push_back(Particles_struct());
	}

}

/* Deconstructor to safely delete when done. */
Particle::~Particle()
{

}

/* Setup the shape of the particle. */
void Particle::setupGeometry()
{
	GLfloat vertices_array[] = {
		// Front vertices
		-SIZE, -SIZE,  SIZE,
		SIZE, -SIZE,  SIZE,
		SIZE,  SIZE,  SIZE,
		-SIZE,  SIZE,  SIZE,
		// Back vertices
		-SIZE, -SIZE, -SIZE,
		SIZE, -SIZE, -SIZE,
		SIZE,  SIZE, -SIZE,
		-SIZE,  SIZE, -SIZE
	};
	GLuint indices_array[] = {  // Note that we start from 0!
		//Right face
		3, 2, 6,
		6, 7, 3,
		//Front face
		0, 1, 2,
		2, 3, 0,
		//Top face
		1, 5, 6,
		6, 2, 1,
		//Back face
		7, 6, 5,
		5, 4, 7,
		//Bottom face
		4, 0, 3,
		3, 7, 4,
		//Left face
		4, 5, 1,
		1, 0, 4,
	};
	//Front vertices.
	this->vertices.push_back(glm::vec3(-SIZE, -SIZE, SIZE));
	this->vertices.push_back(glm::vec3(SIZE, -SIZE, SIZE));
	this->vertices.push_back(glm::vec3(SIZE, SIZE, SIZE));
	this->vertices.push_back(glm::vec3(-SIZE, SIZE, SIZE));
	//Back vertices.
	this->vertices.push_back(glm::vec3(-SIZE, -SIZE, -SIZE));
	this->vertices.push_back(glm::vec3(SIZE, -SIZE, -SIZE));
	this->vertices.push_back(glm::vec3(SIZE, SIZE, -SIZE));
	this->vertices.push_back(glm::vec3(-SIZE, SIZE, -SIZE));
	//Faces.
	for (int i = 35; i >= 0; i--)//35
	{
		this->indices.push_back(indices_array[i]);
	}
}

/* Setup the particle. */
void Particle::setupParticle()
{
	//Create buffers/arrays.
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	//Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO); //Bind vertex array object.

	glBindBuffer(GL_ARRAY_BUFFER, VBO); //Bind vertex buffer.
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * 3 * sizeof(GLfloat), &this->vertices[0], GL_STATIC_DRAW);//Set vertex buffer to vertices.

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); //Bind the EBO for the indices (faces). 
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(int), &this->indices[0], GL_STATIC_DRAW);

	//Vertex Positions.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	glBindBuffer(GL_ARRAY_BUFFER, 0); //Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind.

	glBindVertexArray(0); //Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO.
}

/* Returns if the particle is alive and performs 'animation' on those that are alive. */
void Particle::update()
{
	GLuint nr_new_particles = 1;
	float dt = 0.001;
	//Add new particles when dead to constantly generate more particles.
	for (GLuint i = 0; i < nr_new_particles; ++i)
	{
		int unusedParticle = FirstUnusedParticle();
		RespawnParticle(particles[unusedParticle]);
	}
	//Update all particles to determine it's new life.
	for (GLuint i = 0; i < nr_particles; ++i)
	{
		Particles_struct &cur_particle = particles[i];
		cur_particle.Life -= dt;//Reduce it's life.
		//If the particle is alive, we update it.
		if (cur_particle.Life > 0.0f)
		{
			animate(cur_particle);
		}
	}
}

/* Animate the particle. */
void Particle::animate(Particles_struct &particle)
{
	double timer = glfwGetTime();
	float num_1 = (float)std::fmod(timer, RANDOM_HEIGHT);
	//Set to variables and update so it oscillates.
	if (num_1 > (RANDOM_HEIGHT/2)) num_1 = RANDOM_HEIGHT - num_1;
	particle.Position.y = WATER_HEIGHT + 0.01*num_1;
}

/* Find the most recently unused particle to restart. */
GLuint Particle::FirstUnusedParticle()
{
	//Search from last used particle, this will usually return almost instantly
	for (GLuint i = lastUsedParticle; i < nr_particles; ++i) {
		if (particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	//Otherwise, do a linear search
	for (GLuint i = 0; i < lastUsedParticle; ++i) {
		if (particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	//Override first particle if all others are alive
	lastUsedParticle = 0;
	return 0;
}

/* Resets the particle to an "origin" state. */
void Particle::RespawnParticle(Particles_struct &particle)
{
	//Generate random numbers.
	GLfloat rand_color = 0.6 + ((rand() % 100) / 100.0f);

	//Set it's life back to 1.
	particle.Life = 1.0f;
	//Update it's position.
	float dirX = (float)(rand() % RANDOM_SIZE) - (RANDOM_SIZE/2);
	float dirY = particle.Position.y;
	float dirZ = (float)(rand() % RANDOM_SIZE) - (RANDOM_SIZE/2);
	particle.Position = glm::vec3(dirX, dirY, dirZ);
	//Set it's color.
	particle.Color = glm::vec4(0.0f, rand_color, 1.0f, 1.0f);
	particle.Velocity = glm::vec3(0.0f);
}

/* Draw the Particle. */
void Particle::draw(GLuint shaderProgram)
{
	glm::mat4 MVP = Window::P * Window::V * this->toWorld;
	glm::mat4 model = this->toWorld;//We don't really need this, but we'll pass it through just in case.
	glm::mat4 view = glm::mat4(glm::mat3(Window::V));//Remove translation from the view matrix.
	glm::mat4 projection = Window::P;
	//Set MVP(Total calculated, easier to multiply in the shader) for the shader.
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	//Set individual components for shader calculations (Model, View, Projection).
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
	//Update viewPos.
	glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), Window::camera_pos.x, Window::camera_pos.y, Window::camera_pos.z);
	//Draw.
	glBindVertexArray(VAO);//Bind the vertex.
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	for (Particles_struct particle : this->particles)
	{
		if (particle.Life > 0.0f)
		{
			glBindVertexArray(VAO);//Bind the vertex.
			//Perform any updates per particle here.
			glUniform3f(glGetUniformLocation(shaderProgram, "offset"), particle.Position.x, particle.Position.y, particle.Position.z);
			glUniform4f(glGetUniformLocation(shaderProgram, "p_color"), particle.Color.x, particle.Color.y, particle.Color.z, particle.Color.w);
			//Draw the element.
			if (Window::toon_shading)
			{
				glDrawElements(GL_TRIANGLES, (GLsizei)6, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}
		}
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindVertexArray(0);//Unbind vertex.
}
