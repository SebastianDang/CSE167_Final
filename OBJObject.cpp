#include "OBJObject.h"
#include "Window.h"
#include <iostream>
#include <fstream>

#define DEBUG 0
#define MOVE_STEP 1.0f
#define SCALE_UP 1.2f
#define SCALE_DOWN 0.8f

OBJObject::OBJObject(const char *filepath) 
{
	//Initialize World
	this->toWorld = glm::mat4(1.0f);
	//Parse the object @ filepath.
	this->parse(filepath);
	//Setup the object.
	this->setupObject();
}

OBJObject::~OBJObject()
{
	// Properly de-allocate all resources once they've outlived their purpose
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

/* Render the object in modern openGL using a shader program. */
void OBJObject::draw(GLuint shaderProgram)
{
	//Calculate combination of the model (toWorld), view (camera inverse), and perspective matrices. Send to shader.
	glm::mat4 MVP = Window::P * Window::V * toWorld;
	glm::mat4 model = this->toWorld;
	glm::mat4 view = Window::V;
	glm::mat4 projection = Window::P;
	//Set MVP for the shader.
	GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	//Set individual components for shader calculations.
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

	//Setup the fragment shader.

	glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), 0.0f, 0.0f, 20.0f);

	//Setup the material.
	/* Polished gold 
	glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), 0.24725f, 0.2245f, 0.0645f);
	glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), 0.34615f, 0.3143f, 0.0903f);
	glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), 0.797357f, 0.723991f, 0.208006f);
	glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 83.2f);
	*/
	/* Obsidian 
	glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), 0.05375f, 0.05f, 0.06625f);
	glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), 0.18275f, 0.17f, 0.22525f);
	glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), 0.332741f, 0.328634f, 0.346435f);
	glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 38.4f);
	*/ 
	/* Jade */
	glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), 0.135f, 0.2225f, 0.1575f);
	glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), 0.54f, 0.89f, 0.63f);
	glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), 0.316228f, 0.316228f, 0.316228f);
	glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 12.8f);

	//Directional Light
	glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
	glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.ambient"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.diffuse"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.specular"), 1.0f, 1.0f, 1.0f);






















	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, (GLsizei)this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

/* Scale the object up. */
void OBJObject::scaleUp()
{
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(SCALE_UP));
	this->toWorld *= scaleMatrix;
}

/* Scale the object down. */
void OBJObject::scaleDown()
{
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(SCALE_DOWN));
	this->toWorld *= scaleMatrix;
}

/* Fix Orientation, position in space (World). */
void OBJObject::reset()
{
	toWorld = glm::mat4(1.0f);
}

/* Trackball mapping used to map coordinates in a sphere instead of window coordinates x and y. */
glm::vec3 OBJObject::trackBallMapping(glm::vec3 point)    // The CPoint class is a specific Windows class. Either use separate x and y values for the mouse location, or use a Vector3 in which you ignore the z coordinate.
{
	glm::vec3 trackball_p;    // Vector v is the synthesized 3D position of the mouse location on the trackball
	float depth;     // this is the depth of the mouse location: the delta between the plane through the center of the trackball and the z position of the mouse
	trackball_p.x = (float)((2.0*point.x - Window::width) / Window::width);   // this calculates the mouse X position in trackball coordinates, which range from -1 to +1
	trackball_p.y = (float)((Window::height - 2.0*point.y) / Window::height);   // this does the equivalent to the above for the mouse Y position
	trackball_p.z = 0.0;   // initially the mouse z position is set to zero, but this will change below
	depth = (float)glm::length(trackball_p);    // this is the distance from the trackball's origin to the mouse location, without considering depth (=in the plane of the trackball's origin).
	depth = (float)((depth<1.0) ? depth : 1.0);   // this limits d to values of 1.0 or less to avoid square roots of negative values in the following line
	trackball_p.z = (float)(sqrtf((float)1.001 - (float)(depth*depth)));  // this calculates the Z coordinate of the mouse position on the trackball, based on Pythagoras: v.z*v.z + d*d = 1*1
	trackball_p = glm::normalize(trackball_p); // Still need to normalize, since we only capped d, not v.
	return trackball_p;  // return the mouse location on the surface of the trackball
}

/* Rotate around the middle of the screen based on mouse drag from v to w. */
void OBJObject::rotate(glm::vec3 v, glm::vec3 w)
{
	glm::vec3 direction = w - v;
	float velocity = (float)glm::length(direction);
	if (velocity > 0.0001) 
	{
		//Calculate Rotation Axis.
		glm::vec3 rotAxis = glm::cross(v, w);
		//Calculate Rotation Angle.
		float rot_angle = acos(glm::dot(v, w));
		//Calculate Rotation.
		glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), (rot_angle / 180.0f * glm::pi<float>()), rotAxis);
		this->toWorld = rotate*this->toWorld;
	}
}

void OBJObject::translate(glm::vec3 v, glm::vec3 w)
{
	glm::vec3 translate_v = glm::vec3(w.x - v.x, v.y - w.y, 0.0f);//Since x- and x+ are from left to right, y must be inverted so that y- and y+ are from bottom to top.
	translate_v *= (float)(1 / (float)Window::width);//Scale the translation.
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), translate_v);
	this->toWorld = translate*this->toWorld;
}

void OBJObject::zoom(double y) 
{
	glm::vec3 translate_v = glm::vec3(0.0f, 0.0f, y);//Translate only in the Y coordinate.
	translate_v *= (float)(1 / (float)2);//Scale the translation.
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), translate_v);
	this->toWorld = translate*this->toWorld;
}