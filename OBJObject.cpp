#include "OBJObject.h"
#include "Window.h"

using namespace std;

#define RUN_SPEED  5.0f
#define TURN_SPEED  10.0f


/* Initialize the object, parse it and set up buffers. */
OBJObject::OBJObject(const char *filepath, int material) 
{
	//Initialize movement.
	this->currentDirection = glm::vec3(0.0f, 0.0f, 1.0f);
	this->currentSpeed = 0.0f;
	this->currentTurnSpeed = 0.0f;
	//Initialize World and material.
	this->toWorld = glm::mat4(1.0f);//Default at the origin.
	this->toWorld[3] = glm::vec4(0.0f, 0.0f, 20.0f, 1.0f);
	this->material = material;//Set the material to the passed in material number!
	//Parse the object @ filepath.
	this->parse(filepath);
	//Setup the object.
	this->setupObject();
	//Setup the object material.
	this->setupMaterial();
}

/* Deconstructor to safely delete when finished. */
OBJObject::~OBJObject()
{
	//Properly de-allocate all resources once they've outlived their purpose.
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

/* Populate the face indices, vertices, and normals vectors with the OBJ Object data. */
void OBJObject::parse(const char *filepath)
{
	//Initialize min, max, scale values for each coordinate.
	float minX, minY, minZ, maxX, maxY, maxZ, avgX, avgY, avgZ, scale_v;
	minX = INFINITY, minY = INFINITY, minZ = INFINITY;//Minimum set to infinity so first value is always inputted.
	maxX = -INFINITY, maxY = -INFINITY, maxZ = -INFINITY;//Maximum set to -infinity so first value is always inputted.
	scale_v = -INFINITY;//Same for scale, used to find "max" scale or the longest axis. This ensures the ranges of vertices are [-1, 1].
	//Open the file for reading called objFile.
	std::FILE * objFile = fopen(filepath, "r");
	if (objFile == NULL) return;
	//Read the file until the end; "# are commments to be ignored".
	while (1) {
		char buf[BUFSIZ];
		int check = fscanf(objFile, "%s", buf);
		if (check == EOF) break;
		//Read in lines that start with "v". Add into vertices.
		if (strcmp(buf, "v") == 0) {
			glm::vec3 vertex;
			fscanf(objFile, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vertices.push_back(vertex);
			//Calculate min, max for x, y, z.
			if (vertex.x < minX) { minX = vertex.x; }
			if (vertex.y < minY) { minY = vertex.y; }
			if (vertex.z < minZ) { minZ = vertex.z; }
			if (vertex.x > maxX) { maxX = vertex.x; }
			if (vertex.y > maxY) { maxY = vertex.y; }
			if (vertex.z > maxZ) { maxZ = vertex.z; }
			continue;
		}
		//Read in lines that start with "vn". Add into normals.
		if (strcmp(buf, "vn") == 0) {
			glm::vec3 normalVertex;
			fscanf(objFile, "%f %f %f\n", &normalVertex.x, &normalVertex.y, &normalVertex.z);
			normals.push_back(normalVertex);
			continue;
		}
		//Read in lines that start with "f". Add into indices.
		if (strcmp(buf, "f") == 0) {//Note: Read only left or right side since they are identical on both sides.
			unsigned int faces_v[3], faces_vn[3];
			fscanf(objFile, "%d//%d %d//%d %d//%d\n", &faces_v[0], &faces_vn[0], &faces_v[1], &faces_vn[1], &faces_v[2], &faces_vn[2]);
			indices.push_back(faces_v[0] - 1);
			indices.push_back(faces_v[1] - 1);
			indices.push_back(faces_v[2] - 1);
			continue;
		}
	}
	fclose(objFile);
	//Calculate average x, y, z.
	avgX = (minX + maxX) / 2;
	avgY = (minY + maxY) / 2;
	avgZ = (minX + maxZ) / 2;
	//Calculate scale value.
	float scale_x = (maxX - avgX);
	float scale_y = (maxY - avgY);
	float scale_z = (maxZ - avgZ);
	if (scale_x > scale_v) { scale_v = scale_x; }
	if (scale_y > scale_v) { scale_v = scale_y; }
	if (scale_z > scale_v) { scale_v = scale_z; }
	//Subtract the average to center all objects and multiply by (1/scale) to bring them down to size.
	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		vertices[i].x = vertices[i].x - avgX;
		vertices[i].y = vertices[i].y - avgY;
		vertices[i].z = vertices[i].z - avgZ;
		vertices[i] *= (1 / (scale_v));
		//Throw everything into a container to hold all values.
		Container container;
		container.vertex = vertices[i];
		container.normal = normals[i];
		container.texCoord = glm::vec2(0.0f, 0.0f);
		containers.push_back(container);
	}
}

/* Setup the object for modern openGL rendering. */
void OBJObject::setupObject()
{
	//Create buffers/arrays.
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	//Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO); //Bind vertex array object.

	glBindBuffer(GL_ARRAY_BUFFER, VBO); //Bind Container buffer.
	glBufferData(GL_ARRAY_BUFFER, this->containers.size() * sizeof(Container), &this->containers[0], GL_STATIC_DRAW); //Set vertex buffer to the Container.
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); //Bind indices buffer.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(int), &this->indices[0], GL_STATIC_DRAW);
	
	//Vertex Positions.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,//This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, //This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component).
		GL_FLOAT, //What type these components are.
		GL_FALSE, //GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't.
		sizeof(Container), //Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between.
		(GLvoid*)0); //Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	//Vertex Normals.
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Container), (GLvoid*)offsetof(Container, normal));
	
	//Vertex Texture Coords.
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Container), (GLvoid*)offsetof(Container, texCoord));

	//Unbind.
	glBindBuffer(GL_ARRAY_BUFFER, 0); //Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind.
	glBindVertexArray(0); //Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO.
}

/* Setup the material of the object. We can define different materials here as well! */
void OBJObject::setupMaterial()
{
	if (this->material == 1)
	{	/* Polished gold */
		this->objMaterial.ambient = glm::vec3(0.24725f, 0.2245f, 0.0645f);
		this->objMaterial.diffuse = glm::vec3(0.34615f, 0.3143f, 0.0903f);
		this->objMaterial.specular = glm::vec3(0.797357f, 0.723991f, 0.208006f);
		this->objMaterial.shininess = 83.2f;
	}
	else if (this->material == 2)
	{   /* Obsidian */
		this->objMaterial.ambient = glm::vec3(0.05375f, 0.05f, 0.06625f);
		this->objMaterial.diffuse = glm::vec3(0.18275f, 0.17f, 0.22525f);
		this->objMaterial.specular = glm::vec3(0.332741f, 0.328634f, 0.346435f);
		this->objMaterial.shininess = 38.4f;
	}
	else if (this->material == 3)
	{	/* Jade */
		this->objMaterial.ambient = glm::vec3(0.135f, 0.2225f, 0.1575f);
		this->objMaterial.diffuse = glm::vec3(0.54f, 0.89f, 0.63f);
		this->objMaterial.specular = glm::vec3(0.316228f, 0.316228f, 0.316228f);
		this->objMaterial.shininess = 12.8f;
	}
	else if (this->material == 4)
	{   /* Red Obsidian */
		this->objMaterial.ambient = glm::vec3(1.0f, 0.0f, 0.0f);
		this->objMaterial.diffuse = glm::vec3(0.18275f, 0.17f, 0.22525f);
		this->objMaterial.specular = glm::vec3(0.332741f, 0.328634f, 0.346435f);
		this->objMaterial.shininess = 38.4f;
	}
	else
	{	/* None selected, set to black */
		this->objMaterial.ambient = glm::vec3(1.0f, 0.0f, 0.0f);
		this->objMaterial.diffuse = glm::vec3(0.01f, 0.01f, 0.01f);
		this->objMaterial.specular = glm::vec3(0.5f, 0.5f, 0.5f);
		this->objMaterial.shininess = 32.0f;
	}
}

/* Update Material if needed from any changes in the material struct */
void OBJObject::updateMaterial(GLuint shaderProgram) 
{
	glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), objMaterial.ambient.x, objMaterial.ambient.y, objMaterial.ambient.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), objMaterial.diffuse.x, objMaterial.diffuse.y, objMaterial.diffuse.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), objMaterial.specular.x, objMaterial.specular.y, objMaterial.specular.z);
	glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), objMaterial.shininess);
	//Update the reflection intensity.
	glUniform1f(glGetUniformLocation(shaderProgram, "reflect_intensity"), objMaterial.shininess / 100.0f);
}

/* Render the object in modern openGL using a shader program. */
void OBJObject::draw(GLuint shaderProgram)
{
	//Calculate combination of the model (toWorld), view (camera inverse), and perspective matrices. Send to shader.
	glm::mat4 MVP = Window::P * Window::V * toWorld;
	glm::mat4 model = this->toWorld;
	glm::mat4 view = Window::V;
	glm::mat4 projection = Window::P;
	//Set MVP(Total calculated, easier to multiply in the shader) for the shader.
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	//Set individual components for shader calculations (Model, View, Projection).
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
	//Update the viewPosition.
	glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), Window::camera_pos.x, Window::camera_pos.y, Window::camera_pos.z);
	//Update the material.
	updateMaterial(shaderProgram);
	//Bind for rendering.
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, (GLsizei)this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void OBJObject::movement()
{

}

void OBJObject::W_movement()
{
	glm::vec3 current_position = glm::vec3(this->toWorld[3]);
	glm::vec3 displacement = this->currentDirection * RUN_SPEED;
	glm::vec3 new_position = current_position + displacement;
	this->toWorld[3] = glm::vec4(new_position, 1.0f);
}

void OBJObject::A_movement()
{
	glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), (TURN_SPEED / 180.0f * glm::pi<float>()), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec4 current_direction = glm::vec4(this->currentDirection, 1.0f);
	current_direction = rotate * current_direction;
	glm::vec3 new_direction = glm::vec3(current_direction);
	this->currentDirection = glm::normalize(new_direction);
	this->toWorld = toWorld * rotate;
}

void OBJObject::S_movement()
{
	glm::vec3 current_position = glm::vec3(this->toWorld[3]);
	glm::vec3 displacement = this->currentDirection * RUN_SPEED;
	glm::vec3 new_position = current_position - displacement;
	this->toWorld[3] = glm::vec4(new_position, 1.0f);
}

void OBJObject::D_movement()
{
	glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), (-TURN_SPEED / 180.0f * glm::pi<float>()), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec4 current_direction = glm::vec4(this->currentDirection, 1.0f);
	current_direction = rotate * current_direction;
	glm::vec3 new_direction = glm::vec3(current_direction);
	this->currentDirection = glm::normalize(new_direction);
	this->toWorld = toWorld * rotate;
}

