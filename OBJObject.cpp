#include "OBJObject.h"
#include "Window.h"
#include <iostream>
#include <fstream>

#define MOVE_STEP 1.0f
#define ORBIT_STEP 5.0f
#define SPIN_STEP 1.0f
#define POINT_SIZE_STEP 1.0f
#define POINT_SIZE_MAX	10000.0f
#define POINT_SIZE_MIN	0.0f

OBJObject::OBJObject(const char *filepath) 
{
	//Initialize World, Angle, Orbit Angle, Point Size.
	this->angle = 0.0f;
	this->orbitAngle = 0.0f;
	this->pointSize = 1.0f;
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

void OBJObject::parse(const char *filepath)
{
	//Populate the face indices, vertices, and normals vectors with the OBJ Object data
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
			//indices.push_back(faces_vn[0]);
			//indices.push_back(faces_vn[1]);
			//indices.push_back(faces_vn[2]);
			continue;
		}
	}
	fclose(objFile);
}

void OBJObject::setupObject()
{
	// Create buffers/arrays
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	// For now, we only bind vertices and indices (faces)
	glBindVertexArray(VAO); // Bind vertex array object

	glBindBuffer(GL_ARRAY_BUFFER, VBO); // Bind vertex buffer
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(glm::vec3), &this->vertices[0], GL_STATIC_DRAW); // Set vertex buffer to vertices
	//glBufferData(GL_ARRAY_BUFFER, this->normals.size() * sizeof(glm::vec3), &this->normals, GL_STATIC_DRAW); // Set vertex buffer to normals

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // Bind 
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(int), &this->indices[0], GL_STATIC_DRAW);
	
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	/*
	//Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		sizeof(Vertex), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	//Vertex Normals (Colors / RGB)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 1. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an r, g, and b component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		sizeof(Vertex), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)offsetof(Vertex, Normal)); // Offset of the first vertex's component.

    */
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

}

void OBJObject::draw(GLuint shaderProgram)
{
	// Calculate combination of the model (toWorld), view (camera inverse), and perspective matrices
	glm::mat4 MVP = Window::P * Window::V * toWorld;
	// We need to calculate this because as of GLSL version 1.40 (OpenGL 3.1, released March 2009), gl_ModelViewProjectionMatrix has been
	// removed from the language. The user is expected to supply this matrix to the shader when using modern OpenGL.
	GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	
}

void OBJObject::draw()
{
	glMatrixMode(GL_MODELVIEW);
	//Push a save state onto the matrix stack, and multiply in the toWorld matrix
	glPushMatrix();
	glMultMatrixf(&(toWorld[0][0]));

	/*
	glBegin(GL_POINTS);
	// Loop through all the vertices of this OBJ Object and render them
	for (unsigned int i = 0; i < vertices.size(); ++i)
	{
	glm::vec3 normalized = glm::normalize(normals[i]);//Normalize for the colors.
	float red = normalized.x;
	float green = normalized.y;
	float blue = normalized.z;

	if (red < 0.0f) { red = 0.0f + (-0.5f)*red; }
	else red = 0.0f + (0.5f)*red;

	if (green < 0.0f) { green = 0.0f + (-0.5f)*green; }
	else green = 0.0f + (0.5f)*green;

	if (blue < 0.0f) { blue = 0.0f + (-0.5f)*blue; }
	else blue = 0.0f + (0.5f)*blue;

	glColor3f(red, green, blue);

	glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);//Draw the vertex
	}
	glEnd();
	*/

	glBegin(GL_TRIANGLES);
	for (unsigned int i = 0; i < indices.size(); i++)
	{
		glColor3f(normals[indices[i]].x, normals[indices[i]].y, normals[indices[i]].z);
		glNormal3f(normals[indices[i]].x, normals[indices[i]].y, normals[indices[i]].z);
		glVertex3f(vertices[indices[i]].x, vertices[indices[i]].y, vertices[indices[i]].z);
	}
	glEnd();
	// Pop the save state off the matrix stack
	// This will undo the multiply we did earlier
	glPopMatrix();
}

void OBJObject::update()
{
	//Update any point size changes.
	glPointSize(this->pointSize);
	this->angle += SPIN_STEP;//Current Spin Angle.
	if (this->angle > 360.0f || this->angle < -360.0f) this->angle = 0.0f;
	//Spins.
	spin(this->angle);
}

void OBJObject::spin(float deg)
{
	this->toWorld *= glm::rotate(glm::mat4(SPIN_STEP), 1.0f / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
}

void OBJObject::pointUp() {
	this->pointSize += POINT_SIZE_STEP;
	if (this->pointSize >= POINT_SIZE_MAX) {//Maintain maximum point size of GL_POINT_SIZE_MAX.
		this->pointSize = POINT_SIZE_MAX;
	}
}

void OBJObject::pointDown() {
	this->pointSize -= POINT_SIZE_STEP;
	if (this->pointSize <= POINT_SIZE_MIN) {//Maintain minimum point size of GL_POINT_SIZE_MIN.
		this->pointSize = POINT_SIZE_MIN;
	}
}

void OBJObject::left()
{
	glm::vec4 originVec4 = glm::vec4(toWorld[3]);
	glm::vec3 originVec3 = glm::vec3(originVec4);
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), -originVec3)*glm::rotate(glm::mat4(1.0f), -this->angle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f))*glm::rotate(glm::mat4(1.0f), -this->orbitAngle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 translateInverse = glm::inverse(translate);

	this->toWorld *= translate;
	this->toWorld *= glm::translate(glm::mat4(1.0f), glm::vec3(-MOVE_STEP, 0.0f, 0.0f));
	this->toWorld *= translateInverse;
}

void OBJObject::right()
{
	glm::vec4 originVec4 = glm::vec4(toWorld[3]);
	glm::vec3 originVec3 = glm::vec3(originVec4);
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), -originVec3)*glm::rotate(glm::mat4(1.0f), -this->angle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f))*glm::rotate(glm::mat4(1.0f), -this->orbitAngle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 translateInverse = glm::inverse(translate);

	this->toWorld *= translate;
	this->toWorld *= glm::translate(glm::mat4(1.0f), glm::vec3(MOVE_STEP, 0.0f, 0.0f));
	this->toWorld *= translateInverse;
}

void OBJObject::up()
{
	glm::vec4 originVec4 = glm::vec4(toWorld[3]);
	glm::vec3 originVec3 = glm::vec3(originVec4);
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), -originVec3)*glm::rotate(glm::mat4(1.0f), -this->angle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f))*glm::rotate(glm::mat4(1.0f), -this->orbitAngle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 translateInverse = glm::inverse(translate);

	this->toWorld *= translate;
	this->toWorld *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, MOVE_STEP, 0.0f));
	this->toWorld *= translateInverse;
}

void OBJObject::down()
{
	glm::vec4 originVec4 = glm::vec4(toWorld[3]);
	glm::vec3 originVec3 = glm::vec3(originVec4);
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), -originVec3)*glm::rotate(glm::mat4(1.0f), -this->angle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f))*glm::rotate(glm::mat4(1.0f), -this->orbitAngle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 translateInverse = glm::inverse(translate);

	this->toWorld *= translate;
	this->toWorld *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -MOVE_STEP, 0.0f));
	this->toWorld *= translateInverse;
}

void OBJObject::in()
{
	glm::vec4 originVec4 = glm::vec4(toWorld[3]);
	glm::vec3 originVec3 = glm::vec3(originVec4);
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), -originVec3)*glm::rotate(glm::mat4(1.0f), -this->angle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f))*glm::rotate(glm::mat4(1.0f), -this->orbitAngle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 translateInverse = glm::inverse(translate);

	this->toWorld *= translate;
	this->toWorld *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -MOVE_STEP));
	this->toWorld *= translateInverse;
}

void OBJObject::out()
{
	glm::vec4 originVec4 = glm::vec4(toWorld[3]);
	glm::vec3 originVec3 = glm::vec3(originVec4);
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), -originVec3)*glm::rotate(glm::mat4(1.0f), -this->angle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f))*glm::rotate(glm::mat4(1.0f), -this->orbitAngle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 translateInverse = glm::inverse(translate);

	this->toWorld *= translate;
	this->toWorld *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, MOVE_STEP));
	this->toWorld *= translateInverse;
}

void OBJObject::scaleUp()
{
	//Scale by World.
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
	this->toWorld *= scaleMatrix;
}

void OBJObject::scaleDown()
{
	//Scale by World.
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
	this->toWorld *= scaleMatrix;
}

void OBJObject::orbitCW()
{
	this->orbitAngle -= ORBIT_STEP;
	glm::vec4 originVec4 = glm::vec4(toWorld[3]);
	glm::vec3 originVec3 = glm::vec3(originVec4);

	glm::mat4 orbit = glm::rotate(glm::mat4(1.0f), -ORBIT_STEP / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 orbitI = glm::rotate(glm::mat4(1.0f), ORBIT_STEP / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 orbitInverse = glm::inverse(orbit);

	glm::mat4 translate = glm::translate(glm::mat4(1.0f), -originVec3)*orbitInverse*glm::rotate(glm::mat4(1.0f), -this->angle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 translateInverse = glm::inverse(translate);

	this->toWorld *= translate;
	this->toWorld = orbit*toWorld;
	this->toWorld *= translateInverse;
}

void OBJObject::orbitCCW()
{
	this->orbitAngle += ORBIT_STEP;//Update Angle.
	glm::vec4 originVec4 = glm::vec4(toWorld[3]);
	glm::vec3 originVec3 = glm::vec3(originVec4);

	glm::mat4 orbit = glm::rotate(glm::mat4(1.0f), ORBIT_STEP / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 orbitI = glm::rotate(glm::mat4(1.0f), -ORBIT_STEP / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 orbitInverse = glm::inverse(orbit);
	
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), -originVec3)*orbitInverse*glm::rotate(glm::mat4(1.0f), -this->angle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 translateInverse = glm::inverse(translate);

	this->toWorld *= translate;
	this->toWorld = orbit*toWorld;
	this->toWorld *= translateInverse;
}

void OBJObject::reset()
{
	//Reset any rotations and point size (World).
	this->angle = 0.0f;
	this->orbitAngle = 0.0f;
	this->pointSize = 1.0f;
	//Fix Orientation, position in space (World).
	toWorld = glm::mat4(1.0f);
}