#include "OBJObject.h"
#include <iostream>
#include <fstream>

#define MOVE_STEP 1.0f
#define ORBIT_STEP 5.0f
#define SPIN_STEP 1.0f
#define POINT_SIZE_STEP 1.0f

OBJObject::OBJObject(const char *filepath) 
{
	toWorld = glm::mat4(1.0f);
	this->angle = 0.0f;
	this->orbitAngle = 0.0f;
	this->pointSize = 1.0f;
	this->scale = 0.0f;
	parse(filepath);
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
	}
	fclose(objFile);
}

void OBJObject::draw() 
{
	glMatrixMode(GL_MODELVIEW);

	// Push a save state onto the matrix stack, and multiply in the toWorld matrix
	glPushMatrix();
	glMultMatrixf(&(toWorld[0][0]));

	update();
	
	glBegin(GL_POINTS);
	// Loop through all the vertices of this OBJ Object and render them
	for (unsigned int i = 0; i < vertices.size(); ++i) 
	{
		glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
		glm::vec3 normalized = glm::normalize(normals[i]);
		glColor3f(normalized.x, normalized.y, normalized.z);
	}
	glEnd();
	// Pop the save state off the matrix stack
	// This will undo the multiply we did earlier
	glPopMatrix();
}

void OBJObject::update()
{
	glPointSize(this->pointSize);
	//Angle update.
	this->angle += SPIN_STEP;
	if (this->angle > 360.0f || this->angle < -360.0f) this->angle = 0.0f;
	//Spins and Orbits.
	spin(this->angle);
}

void OBJObject::spin(float deg)
{
	glRotatef(deg, 0.0f, 1.0f, 0.0f);

	/*
	glm::vec4 originVec4 = glm::vec4(toWorld[3]);
	glm::vec3 originVec3 = glm::vec3(originVec4);
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), -originVec3);
	glm::mat4 translateInverse = glm::inverse(translate);
	this->toWorld *= translate;
	this->toWorld *= glm::rotate(glm::mat4(1.0f), 1.0f / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
	this->toWorld *= translateInverse;
	*/
}

void OBJObject::pointUp() {
	this->pointSize += POINT_SIZE_STEP;
	if (this->pointSize >= GL_POINT_SIZE_MAX)//Maintain maximum point size of GL_POINT_SIZE_MAX.
		this->pointSize = GL_POINT_SIZE_MAX;
}

void OBJObject::pointDown() {
	this->pointSize -= POINT_SIZE_STEP;
	if (this->pointSize <= GL_POINT_SIZE_MIN)//Maintain minimum point size of GL_POINT_SIZE_MIN.
		this->pointSize = GL_POINT_SIZE_MIN;
}

void OBJObject::left()
{
	glm::vec4 originVec4 = glm::vec4(toWorld[3]);
	glm::vec3 originVec3 = glm::vec3(originVec4);
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), -originVec3);
	glm::mat4 translateInverse = glm::inverse(translate);
	this->toWorld *= translate;
	this->toWorld *= glm::translate(glm::mat4(1.0f), glm::vec3(-MOVE_STEP, 0.0f, 0.0f));
	this->toWorld *= translateInverse;
}

void OBJObject::right()
{
	glm::vec4 originVec4 = glm::vec4(toWorld[3]);
	glm::vec3 originVec3 = glm::vec3(originVec4);
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), -originVec3);
	glm::mat4 translateInverse = glm::inverse(translate);
	this->toWorld *= translate;
	this->toWorld *= glm::translate(glm::mat4(1.0f), glm::vec3(MOVE_STEP, 0.0f, 0.0f));
	this->toWorld *= translateInverse;
}

void OBJObject::up()
{
	glm::vec4 originVec4 = glm::vec4(toWorld[3]);
	glm::vec3 originVec3 = glm::vec3(originVec4);
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), -originVec3);
	glm::mat4 translateInverse = glm::inverse(translate);
	this->toWorld *= translate;
	this->toWorld *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, MOVE_STEP, 0.0f));
	this->toWorld *= translateInverse;
}

void OBJObject::down()
{
	glm::vec4 originVec4 = glm::vec4(toWorld[3]);
	glm::vec3 originVec3 = glm::vec3(originVec4);
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), -originVec3);
	glm::mat4 translateInverse = glm::inverse(translate);
	this->toWorld *= translate;
	this->toWorld *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -MOVE_STEP, 0.0f));
	this->toWorld *= translateInverse;
}

void OBJObject::in()
{
	glm::vec4 originVec4 = glm::vec4(toWorld[3]);
	glm::vec3 originVec3 = glm::vec3(originVec4);
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), -originVec3);
	glm::mat4 translateInverse = glm::inverse(translate);
	this->toWorld *= translate;
	this->toWorld *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -MOVE_STEP));
	this->toWorld *= translateInverse;
}

void OBJObject::out()
{
	glm::vec4 originVec4 = glm::vec4(toWorld[3]);
	glm::vec3 originVec3 = glm::vec3(originVec4);
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), -originVec3);
	glm::mat4 translateInverse = glm::inverse(translate);
	this->toWorld *= translate;
	this->toWorld *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, MOVE_STEP));
	this->toWorld *= translateInverse;
}

void OBJObject::scaleUp()
{
	this->scale++;
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
	for (unsigned int i = 0; i < vertices.size(); ++i)
	{
		glm::vec4 vec3Point = glm::vec4(vertices[i].x, vertices[i].y, vertices[i].z, 1.0f);
		vec3Point = scaleMatrix * vec3Point;
		glm::vec3 transformedVec3 = glm::vec3(vec3Point);
		vertices[i] = transformedVec3;
		glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
	}
}

void OBJObject::scaleDown()
{
	this->scale--;
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
	for (unsigned int i = 0; i < vertices.size(); ++i)
	{
		glm::vec4 vec3Point = glm::vec4(vertices[i].x, vertices[i].y, vertices[i].z, 1.0f);
		vec3Point = scaleMatrix * vec3Point;
		glm::vec3 transformedVec3 = glm::vec3(vec3Point);
		vertices[i] = transformedVec3;
		glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
	}
}

void OBJObject::orbitCW()
{
	glm::vec4 originVec4 = glm::vec4(toWorld[3]);
	glm::vec3 originVec3 = glm::vec3(originVec4);
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), -originVec3);
	glm::mat4 translateInverse = glm::inverse(translate);
	this->toWorld *= translate;
	this->toWorld = glm::rotate(glm::mat4(1.0f), -ORBIT_STEP / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f))*toWorld;
	this->toWorld *= translateInverse;
	this->toWorld *= glm::rotate(glm::mat4(1.0f), +ORBIT_STEP / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
}

void OBJObject::orbitCCW()
{
	glm::vec4 originVec4 = glm::vec4(toWorld[3]);
	glm::vec3 originVec3 = glm::vec3(originVec4);
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), -originVec3);
	glm::mat4 translateInverse = glm::inverse(translate);
	this->toWorld *= translate;
	this->toWorld = glm::rotate(glm::mat4(1.0f), +ORBIT_STEP / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f))*toWorld;
	this->toWorld *= translateInverse;
	this->toWorld *= glm::rotate(glm::mat4(1.0f), -ORBIT_STEP / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
}

void OBJObject::reset()
{
	//Fix Scaling (model)
	while (scale > 0) { scaleDown(); }
	while (scale < 0) { scaleUp(); }
	//Reset the Rotation (world)
	this->angle = 0.0f;
	//Fix Orientation, position in space (world)
	toWorld = glm::mat4(1.0f);
}