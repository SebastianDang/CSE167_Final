#pragma once

#include "window.h"
#include "Group.h"
#include "MatrixTransform.h"
#include "Cylinder.h"
#include "Pod.h"
#include "Bear.h"

class Cake
{
private:
	MatrixTransform * Ride;

	OBJObject *cylinder_obj;
	OBJObject *pod_obj;
	OBJObject *bear_obj;

	Bear * bear_cam;

	glm::mat4 translate;
	glm::mat4 scale;
	glm::mat4 rotate;
	glm::mat4 transform;

	glm::mat4 cyl_matrix;
	glm::mat4 pod_matrix;
	glm::mat4 beam_matrix;
	glm::mat4 root_matrix;

	std::vector<MatrixTransform*> cyls;
	std::vector<MatrixTransform*> pods;//Rotate.
	std::vector<MatrixTransform*> arms;//Rotate.
	std::vector<MatrixTransform*> arms_middle;//Extend.
	std::vector<MatrixTransform*> arms_sets;
	std::vector<MatrixTransform*> beams;
	std::vector<MatrixTransform*> beams_middle;//Extend.
	std::vector<MatrixTransform*> levels;//Rotate.
	std::vector<MatrixTransform*> roots;
	std::vector<MatrixTransform*> roots_middle;//Extend.
	std::vector<MatrixTransform*> roots_bottom;//Extend.

	MatrixTransform * createArm(glm::mat4 transform);
	MatrixTransform * createArmSet(glm::mat4 transform);
	MatrixTransform * createBeam(glm::mat4 transform);
	MatrixTransform * createLevel(glm::mat4 transform);
	MatrixTransform * createRide(glm::mat4 transform);
	MatrixTransform * mt_bear_camera;

	void rotation();
	void extension();

public:
	Cake();
	~Cake();
	
	glm::mat4 getCamera();
	
	void draw(GLuint shaderprogram);
	void update();
};
