#include "OBJObject.h"
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
	toWorld = glm::mat4(1.0f);
	this->angle = 0.0f;
	this->orbitAngle = 0.0f;
	this->pointSize = 1.0f;
	//Parse the object @ filepath.
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
		//Read in lines that start with "f". Add into faces.
	}
	fclose(objFile);
}

void OBJObject::draw() 
{
	glMatrixMode(GL_MODELVIEW);
	//Push a save state onto the matrix stack, and multiply in the toWorld matrix
	glPushMatrix();
	glMultMatrixf(&(toWorld[0][0]));
	//Update the Object for Rendering.
	update();

	glBegin(GL_POINTS);
	// Loop through all the vertices of this OBJ Object and render them
	for (unsigned int i = 0; i < vertices.size(); ++i) 
	{
		glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);//Draw the vertex
		
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
	this->toWorld *= glm::rotate(glm::mat4(1.0f), 1.0f / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
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