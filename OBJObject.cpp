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
	//toWorld Matrix
	setWorld();
	//C_inverse Matrix
	setCamera();
	//Parse the object @ filepath.
	parse(filepath);
}

std::vector<unsigned int> OBJObject::getIndices() {
	if (this->indices.size() == 0) {
		this->indices.push_back(0);
	}
	return this->indices;
}

std::vector<glm::vec3> OBJObject::getVertices()
{
	if (this->vertices.size() == 0) {
		this->vertices.push_back(glm::vec3(0.0f));
	}
	return this->vertices;
}

std::vector<glm::vec3> OBJObject::getNormals()
{
	if (this->normals.size() == 0) {
		this->normals.push_back(glm::vec3(0.0f));
	}
	return this->normals;
}

glm::mat4 OBJObject::getWorld() 
{
	return this->toWorld;
}

glm::mat4 OBJObject::getCamera()
{
	return this->c_inverse;
}

glm::mat4 OBJObject::getProjection()
{
	return this->projection;
}

glm::mat4 OBJObject::getViewport()
{
	return this->viewport;
}

float OBJObject::getPointSize() {
	return this->pointSize;
}

void OBJObject::setWorld()
{
	this->toWorld = glm::mat4(1.0f);
}

void OBJObject::setCamera()
{
	glm::vec3 e = glm::vec3(0.0f, 0.0f, 20.0f);
	glm::vec3 d = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	this->c_inverse = glm::lookAt(e, d, up);
}

void OBJObject::setProjection(float window_width, float window_height)
{
	this->projection = glm::perspective(glm::radians(60.0f), (float)window_width / (float)window_height, 1.0f, 1000.0f);
}

void OBJObject::setViewport(float window_width, float window_height)
{
	float viewportX = (window_width - 0.0f) * (0.5f);
	float viewportY = (window_height - 0.0f) * (0.5f);
	float viewportX2 = (window_width + 0.0f) * (0.5f);
	float viewportY2 = (window_height + 0.0f) * (0.5f);
	this->viewport = glm::mat4(viewportX, 0.0f, 0.0f, viewportX2,
		0.0f, viewportY, 0.0f, viewportY2,
		0.0f, 0.0f, 0.5f, 0.5f,
		0.0f, 0.0f, 0.0f, 1.0f);
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
			//fscanf(objFile, "%f %f %f\n", ...colors...);//Colors?
			vertices.push_back(vertex);
		}
		//Read in lines that start with "vn". Add into normals.
		if (strcmp(buf, "vn") == 0) {
			glm::vec3 normalVertex;
			fscanf(objFile, "%f %f %f\n", &normalVertex.x, &normalVertex.y, &normalVertex.z);
			normals.push_back(normalVertex);
		}
		//Read in lines that start with "f". Add into indices.
		if (strcmp(buf, "f") == 0) {
			unsigned int startIndex[3], endIndex[3];
			fscanf(objFile, "%d//%d %d//%d %d//%d\n", &startIndex[0], &endIndex[0], &startIndex[1], &endIndex[1], &startIndex[2], &endIndex[2]);
			indices.push_back(startIndex[0]);
			indices.push_back(startIndex[1]);
			indices.push_back(startIndex[2]);
			indices.push_back(endIndex[0]);
			indices.push_back(endIndex[1]);
			indices.push_back(endIndex[2]);
		}

	}
	fclose(objFile);
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

void OBJObject::draw(GLuint shaderProgram)
{
	// Calculate combination of the model (toWorld), view (camera inverse), and perspective matrices
	glm::mat4 MVP = Window::P * Window::V * toWorld;
	// We need to calculate this because as of GLSL version 1.40 (OpenGL 3.1, released March 2009), gl_ModelViewProjectionMatrix has been
	// removed from the language. The user is expected to supply this matrix to the shader when using modern OpenGL.
	GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	/*
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	*/

	glBegin(GL_TRIANGLES);
	for (unsigned int i = 0; i < indices.size(); i++)
	{
		glColor3f(normals[indices[i]].x, normals[indices[i]].y, normals[indices[i]].z);
		glNormal3f(normals[indices[i]].x, normals[indices[i]].y, normals[indices[i]].z);
		glVertex3f(vertices[indices[i]].x, vertices[indices[i]].y, vertices[indices[i]].z);
	}
	glEnd();


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