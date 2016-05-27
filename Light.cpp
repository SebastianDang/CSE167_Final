#include "Window.h"
#include "Light.h"

#define SPOT_CUTOFF 30.0f

Light::Light()
{
	setupLighting();
}

Light::~Light()
{

}

/* Setup the lighting for the object. */
void Light::setupLighting()
{
	//Directional Light
	this->dirLight.on = 1;
	this->dirLight.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
	this->dirLight.ambient = glm::vec3(1.0f, 1.0f, 1.0f);
	this->dirLight.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	this->dirLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
	//PointLight
	this->pointLight.on = 0;
	this->pointLight.position = glm::vec3(10.0f, 10.0f, 10.0f);
	this->pointLight.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	this->pointLight.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	this->pointLight.specular = glm::vec3(0.8f, 0.8f, 0.8f);
	this->pointLight.quadratic = 0.032f;
	//SpotLight
	this->spotLight.on = 0;
	this->spotLight.position = glm::vec3(10.0f, 10.0f, 10.0f);
	this->spotLight.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	this->spotLight.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	this->spotLight.specular = glm::vec3(0.8f, 0.8f, 0.8f);
	this->spotLight.quadratic = 0.032f;//attenuation
	this->spotLight.direction = -spotLight.position;//This ensures that the spotlight is always pointing at the origin (position away from the origin).
	this->spotLight.spotCutoff = (SPOT_CUTOFF / 180.0f * glm::pi<float>());
	this->spotLight.spotExponent = 1.0f;
}


/* Select which light to be on when pressing 1, 2, 3. */
void Light::updateSelectLighting()
{
	if (light_selection == 1)
	{	//Turn on Directional Light.
		this->dirLight.on = 1;
		this->pointLight.on = 0;
		this->spotLight.on = 0;
	}
	else if (light_selection == 2)
	{	//Turn on Point Light.
		this->dirLight.on = 0;
		this->pointLight.on = 1;
		this->spotLight.on = 0;
	}
	else if (light_selection == 3)
	{	//Turn on Spot Light.
		this->dirLight.on = 0;
		this->pointLight.on = 0;
		this->spotLight.on = 1;
	}
}

/* Update lighting if needed from any changes in the light structs */
void Light::updateLighting(GLuint shaderProgram)
{
	updateSelectLighting();//Turns on the one light that should be on.
	/* Directional Light */
	glUniform1f(glGetUniformLocation(shaderProgram, "dirLight.on"), dirLight.on);
	glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.direction"), dirLight.direction.x, dirLight.direction.y, dirLight.direction.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.ambient"), dirLight.ambient.x, dirLight.ambient.y, dirLight.ambient.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.diffuse"), dirLight.diffuse.x, dirLight.diffuse.y, dirLight.diffuse.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.specular"), dirLight.specular.x, dirLight.specular.y, dirLight.specular.z);
	/* Point light */
	glUniform1f(glGetUniformLocation(shaderProgram, "pointLight.on"), pointLight.on);
	glUniform3f(glGetUniformLocation(shaderProgram, "pointLight.position"), pointLight.position.x, pointLight.position.y, pointLight.position.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "pointLight.ambient"), pointLight.ambient.x, pointLight.ambient.y, pointLight.ambient.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "pointLight.diffuse"), pointLight.diffuse.x, pointLight.diffuse.y, pointLight.diffuse.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "pointLight.specular"), pointLight.specular.x, pointLight.specular.y, pointLight.specular.z);
	glUniform1f(glGetUniformLocation(shaderProgram, "pointLight.quadratic"), pointLight.quadratic);
	/* Spot light */
	glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.on"), spotLight.on);
	glUniform3f(glGetUniformLocation(shaderProgram, "spotLight.position"), spotLight.position.x, spotLight.position.y, spotLight.position.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "spotLight.ambient"), spotLight.ambient.x, spotLight.ambient.y, spotLight.ambient.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "spotLight.diffuse"), spotLight.diffuse.x, spotLight.diffuse.y, spotLight.diffuse.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "spotLight.specular"), spotLight.specular.x, spotLight.specular.y, spotLight.specular.z);
	glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.quadratic"), spotLight.quadratic);
	glUniform3f(glGetUniformLocation(shaderProgram, "spotLight.direction"), spotLight.direction.x, spotLight.direction.y, spotLight.direction.z);
	glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.spotCutoff"), spotLight.spotCutoff);
	glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.spotExponent"), spotLight.spotExponent);
}
