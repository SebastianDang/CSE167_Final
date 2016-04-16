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
	minX = INFINITY, minY = INFINITY, minZ = INFINITY;
	maxX = -INFINITY, maxY = -INFINITY, maxZ = -INFINITY;
	scale_v = -INFINITY;
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
		if (strcmp(buf, "f") == 0) {
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
	//For now, we only bind vertices and indices (faces).
	glBindVertexArray(VAO); // Bind vertex array object

	glBindBuffer(GL_ARRAY_BUFFER, VBO); // Bind vertex buffer
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(glm::vec3), &this->vertices[0], GL_STATIC_DRAW); // Set vertex buffer to vertices

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // Bind indices buffer 
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(int), &this->indices[0], GL_STATIC_DRAW);
	
	//Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,//This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, //This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component).
		GL_FLOAT, //What type these components are.
		GL_FALSE, //GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't.
		sizeof(glm::vec3), //Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between.
		(GLvoid*)0); //Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	glBindBuffer(GL_ARRAY_BUFFER, 0); //Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind.
	glBindVertexArray(0); //Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO.
}

/* Render the object in modern openGL using a shader program. */
void OBJObject::draw(GLuint shaderProgram)
{
	// Calculate combination of the model (toWorld), view (camera inverse), and perspective matrices
	glm::mat4 MVP = Window::P * Window::V * toWorld;
	// We need to calculate this because as of GLSL version 1.40 (OpenGL 3.1, released March 2009), gl_ModelViewProjectionMatrix has been
	// removed from the language. The user is expected to supply this matrix to the shader when using modern OpenGL.
	GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
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
	depth = (float)trackball_p.length();    // this is the distance from the trackball's origin to the mouse location, without considering depth (=in the plane of the trackball's origin)
	depth = (float)((depth<1.0) ? depth : 1.0);   // this limits d to values of 1.0 or less to avoid square roots of negative values in the following line
	trackball_p.z = (float)(sqrtf((float)1.001 - (float)(depth*depth)));  // this calculates the Z coordinate of the mouse position on the trackball, based on Pythagoras: v.z*v.z + d*d = 1*1
	trackball_p = glm::normalize(trackball_p); // Still need to normalize, since we only capped d, not v.
	return trackball_p;  // return the mouse location on the surface of the trackball
}

/* Rotate around the middle of the screen based on mouse drag from v to w. */
void OBJObject::rotate(glm::vec3 v, glm::vec3 w)
{
	glm::vec3 direction = w - v;
	float velocity = (float)direction.length();
	if (velocity > 0.0001) 
	{
		//Calculate Rotation Axis.
		glm::vec3 rotateAxis = glm::cross(v, w);
		glm::vec3 rotAxis = glm::vec3(rotateAxis.x, rotateAxis.y, 0.0f);
		//Calculate Rotation Angle.
		float rot_angle = acos(glm::dot(v, w));
		//Calculate Rotation.
		glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), (rot_angle / 180.0f * glm::pi<float>()), rotAxis);
		this->toWorld = rotate*this->toWorld;
	}
}