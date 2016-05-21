#include "Cake.h"
#include "OBJObject.h"

#include "Window.h"
#include "MatrixTransform.h"
#include "Cylinder.h"
#include "Pod.h"
#include "Bear.h"
#include <memory>

using namespace std;

/* Define a Cake Constructor amd initialize the Cake. */
Cake::Cake()
{
	/* Parse objects. */
	this->cylinder_obj = new OBJObject("cylinder.obj", 2);
	this->pod_obj = new OBJObject("pod.obj", 4);
	this->bear_obj = new OBJObject("bear.obj", 3);
	//this->camera_obj = new OBJObject("", 0);
	/* Initialize all variables. */
	Ride = new MatrixTransform();//Resulting Ride
	mt_bear_camera = new MatrixTransform();//Holds the bear_camera coordinates.
	/* Define the cylinder matrix. */
	this->translate = glm::translate(glm::mat4(1.0f), glm::vec3(2.5f, 0.0f, 0.0f));
	this->scale = glm::scale(glm::mat4(1.0f), glm::vec3(2.5f, 0.25f, 0.25f));
	this->rotate = glm::rotate(glm::mat4(1.0f), (90.0f / 180.0f * glm::pi<float>()), glm::vec3(0.0f, 0.0f, 1.0f));
	this->cyl_matrix = translate * scale * rotate;
	/* Define the pod matrix. */
	this->translate = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.5f, 0.0f));
	this->pod_matrix = translate;
	/* Define the beam matrix. */
	this->translate = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 0.0f));
	this->scale = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 0.5f, 0.5f));
	this->rotate = glm::rotate(glm::mat4(1.0f), (90.0f / 180.0f * glm::pi<float>()), glm::vec3(0.0f, 0.0f, 1.0f));
	this->beam_matrix = translate * scale * rotate;
	/* Define the root matrix. */
	this->scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 9.0f, 1.0f));
	this->root_matrix = scale;
	/* Initialize the ride. */
	MatrixTransform * ride = createRide(glm::mat4(1.0f));
	/* Place a bear inside the pod. */
	MatrixTransform * bear_seat = pods[0];//Make the bear seat one of the pods.
	MatrixTransform * bear_placement = new MatrixTransform();//Placement of the bear relative to the pod.
	this->bear_cam = new Bear(bear_obj);
	bear_placement->addChild(this->bear_cam);
	bear_placement->M = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -0.5f))*glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.9f, 1.0f));
	bear_seat->addChild(bear_placement);
	/* Add to total ride. */
	Ride->addChild(ride);
}

/* Deconstructor to safely delete when finished. */
Cake::~Cake()
{
	delete(cylinder_obj);
	delete(pod_obj);
	delete(bear_obj);
}

/* Returns the camera matrix of the bear. */
glm::mat4 Cake::getCamera()
{
	glm::mat4 model = this->mt_bear_camera->M;
	glm::mat4 rotation_matrix = model;
	rotation_matrix[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	rotation_matrix = glm::transpose(rotation_matrix);

	glm::vec4 translations = model[3];
	glm::mat4 translate_matrix = glm::translate(rotation_matrix, glm::vec3(-translations));
	glm::mat4 move = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.25f))*glm::rotate(glm::mat4(1.0f), (180.0f / 180.0f * glm::pi<float>()), glm::vec3(0.0f, 1.0f, 0.0f));;
	translate_matrix = move*translate_matrix;

	return translate_matrix;
}

/* Create an individual arm. */
MatrixTransform * Cake::createArm(glm::mat4 transform)
{
	/* Initialize MatrixTransform pointers. */
	MatrixTransform * mt_cyl_1 = new MatrixTransform();
	MatrixTransform * mt_cyl_2 = new MatrixTransform();
	MatrixTransform * mt_pod_n = new MatrixTransform();
	MatrixTransform * mt_arm_n = new MatrixTransform();
	MatrixTransform * mt_arm_middle = new MatrixTransform();
	/* mt_cyl */
	Cylinder * cyl_1 = new Cylinder(cylinder_obj);//Create the cylinder object.
	mt_cyl_1->addChild(cyl_1);//Add it to mt_cyl for transformations.
	mt_cyl_1->M = transform*cyl_matrix;
	Cylinder * cyl_2 = new Cylinder(cylinder_obj);//Create the cylinder object.
	mt_cyl_2->addChild(cyl_2);//Add it to mt_cyl for transformations.
	mt_cyl_2->M = transform*cyl_matrix;
	/* mt_pod */
	Pod * pod_n = new Pod(pod_obj);//Create the pod object.
	mt_pod_n->addChild(pod_n);//Add it to mt_pod for transformations.
	mt_pod_n->M = transform*pod_matrix;
	/* mt_arm_middle */
	mt_arm_middle->addChild(mt_cyl_1);
	mt_arm_middle->addChild(mt_pod_n);
	/* mt_arm */
	mt_arm_n->addChild(mt_arm_middle);
	mt_arm_n->addChild(mt_cyl_2);
	/* Update the vectors holding pointers. */
	cyls.push_back(mt_cyl_1);
	pods.push_back(mt_pod_n);
	arms.push_back(mt_arm_n);
	arms_middle.push_back(mt_arm_middle);

	return mt_arm_n;
}

/* Create 1 set of arms. */
MatrixTransform * Cake::createArmSet(glm::mat4 transform)
{
	/* Initialize MatrixTransform pointers. */
	MatrixTransform * mt_arms_n = new MatrixTransform();
	MatrixTransform * mt_arm_1 = new MatrixTransform();
	MatrixTransform * mt_arm_2 = new MatrixTransform();
	MatrixTransform * mt_arm_3 = new MatrixTransform();
	/* mt_arm 1, 2, 3 */
	glm::mat4 arm_1_matrix = glm::mat4(1.0f);
	glm::mat4 arm_2_matrix = glm::rotate(glm::mat4(1.0f), (120.0f / 180.0f * glm::pi<float>()), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 arm_3_matrix = glm::rotate(glm::mat4(1.0f), (240.0f / 180.0f * glm::pi<float>()), glm::vec3(0.0f, 1.0f, 0.0f));
	mt_arm_1 = createArm(glm::mat4(1.0f));
	mt_arm_2 = createArm(glm::mat4(1.0f));
	mt_arm_3 = createArm(glm::mat4(1.0f));
	mt_arm_1->M = arm_1_matrix;
	mt_arm_2->M = arm_2_matrix;
	mt_arm_3->M = arm_3_matrix;
	/* mt_arms */
	mt_arms_n->addChild(mt_arm_1);
	mt_arms_n->addChild(mt_arm_2);
	mt_arms_n->addChild(mt_arm_3);
	/* Update the vectors holding pointers. */
	arms_sets.push_back(mt_arms_n);

	return mt_arms_n;
}

/* Create an individual beam. */
MatrixTransform * Cake::createBeam(glm::mat4 transform)
{
	/* Initialize MatrixTransform pointers. */
	MatrixTransform * mt_beam_n = new MatrixTransform();
	MatrixTransform * mt_beam_middle = new MatrixTransform();
	MatrixTransform * mt_beam_container_1 = new MatrixTransform();
	MatrixTransform * mt_beam_container_2 = new MatrixTransform();
	MatrixTransform * mt_arms_n = new MatrixTransform();
	/* mt_beam */
	Cylinder * cyl_1 = new Cylinder(cylinder_obj);//Create the cylinder object.
	mt_beam_container_1->addChild(cyl_1);//Add it to mt_cyl for transformations.
	mt_beam_container_1->M = transform*beam_matrix;
	Cylinder * cyl_2 = new Cylinder(cylinder_obj);//Create the cylinder object.
	mt_beam_container_2->addChild(cyl_2);//Add it to mt_cyl for transformations.
	mt_beam_container_2->M = transform*beam_matrix;
	/* mt_arms */
	glm::mat4 beam_length = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.5f, 0.0f));
	mt_arms_n = createArmSet(glm::mat4(1.0f));
	mt_arms_n->M = transform*beam_length;
	/* mt_beam_middle */
	mt_beam_middle->addChild(mt_beam_container_1);
	mt_beam_middle->addChild(mt_arms_n);
	/* mt_beam */
	mt_beam_n->addChild(mt_beam_middle);
	mt_beam_n->addChild(mt_beam_container_2);
	/* Update the vectors holding pointers. */
	beams.push_back(mt_beam_n);//Note: May need support here.
	beams_middle.push_back(mt_beam_middle);

	return mt_beam_n;
}

/* Create an individual level. */
MatrixTransform * Cake::createLevel(glm::mat4 transform)
{
	/* Initialize MatrixTransform pointers. */
	MatrixTransform * mt_level_n = new MatrixTransform();
	MatrixTransform * mt_beam_1 = new MatrixTransform();
	MatrixTransform * mt_beam_2 = new MatrixTransform();
	MatrixTransform * mt_beam_3 = new MatrixTransform();
	/* mt_beam 1, 2, 3 */
	glm::mat4 beam_1_matrix = glm::mat4(1.0f);
	glm::mat4 beam_2_matrix = glm::rotate(glm::mat4(1.0f), (120.0f / 180.0f * glm::pi<float>()), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 beam_3_matrix = glm::rotate(glm::mat4(1.0f), (240.0f / 180.0f * glm::pi<float>()), glm::vec3(0.0f, 1.0f, 0.0f));
	mt_beam_1 = createBeam(glm::mat4(1.0f));
	mt_beam_2 = createBeam(glm::mat4(1.0f));
	mt_beam_3 = createBeam(glm::mat4(1.0f));
	mt_beam_1->M = beam_1_matrix;
	mt_beam_2->M = beam_2_matrix;
	mt_beam_3->M = beam_3_matrix;
	/* mt_beams */
	mt_level_n->addChild(mt_beam_1);
	mt_level_n->addChild(mt_beam_2);
	mt_level_n->addChild(mt_beam_3);
	/* Update the vectors holding pointers. */
	levels.push_back(mt_level_n);

	return mt_level_n;
}

/* Create an individual ride. */
MatrixTransform * Cake::createRide(glm::mat4 transform)
{
	/* Initialize MatrixTransform pointers. */
	MatrixTransform * mt_ride_n = new MatrixTransform();
	MatrixTransform * mt_ride_middle = new MatrixTransform();
	MatrixTransform * mt_ride_bottom = new MatrixTransform();
	MatrixTransform * mt_root_1 = new MatrixTransform();
	MatrixTransform * mt_root_2 = new MatrixTransform();
	MatrixTransform * mt_root_3 = new MatrixTransform();
	MatrixTransform * mt_level_1 = new MatrixTransform();
	MatrixTransform * mt_level_2 = new MatrixTransform();
	MatrixTransform * mt_level_3 = new MatrixTransform();
	/* mt_cyl */
	Cylinder * cyl_1 = new Cylinder(cylinder_obj);//Create the cylinder object.
	mt_root_1->addChild(cyl_1);//Add it to mt_cyl for transformations.
	mt_root_1->M = transform*root_matrix;
	Cylinder * cyl_2 = new Cylinder(cylinder_obj);//Create the cylinder object.
	mt_root_2->addChild(cyl_2);//Add it to mt_cyl for transformations.
	mt_root_2->M = transform*root_matrix;
	Cylinder * cyl_3 = new Cylinder(cylinder_obj);//Create the cylinder object.
	mt_root_3->addChild(cyl_3);//Add it to mt_cyl for transformations.
	mt_root_3->M = transform*root_matrix;
	/* mt_levels */
	glm::mat4 level_1_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 6.0f, 0.0f));
	glm::mat4 level_2_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.0f, 0.0f));
	glm::mat4 level_3_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	mt_level_1 = createLevel(glm::mat4(1.0f));
	mt_level_2 = createLevel(glm::mat4(1.0f));
	mt_level_3 = createLevel(glm::mat4(1.0f));
	mt_level_1->M = level_1_matrix;
	mt_level_2->M = level_2_matrix;
	mt_level_3->M = level_3_matrix;
	/* mt_ride_middle */
	mt_ride_middle->addChild(mt_root_1);
	mt_ride_middle->addChild(mt_level_1);
	/* mt_ride_bottom */
	mt_ride_bottom->addChild(mt_root_2);
	mt_ride_bottom->addChild(mt_level_2);
	/* mt_ride_n */
	mt_ride_n->addChild(mt_ride_middle);
	mt_ride_n->addChild(mt_ride_bottom);
	mt_ride_n->addChild(mt_level_3);
	/* Update the vectors holding pointers. */
	roots.push_back(mt_ride_n);
	roots_middle.push_back(mt_ride_middle);
	roots_bottom.push_back(mt_ride_bottom);

	return mt_ride_n;
}

/* Traverse the list of children and call each child node's draw function. */
void Cake::draw(GLuint shaderprogram)
{
	Ride->draw(shaderprogram);
}

/* Update the cake with any animations that occur. */
void Cake::update()
{
	rotation();
	extension();
	Ride->update(glm::mat4(1.0f));
	this->mt_bear_camera->M = this->bear_cam->toDraw->toWorld;//Update the bear camera.
}

/* Update the pods, arms, and center to rotate about it's axis. */
void Cake::rotation()
{
	glm::mat4 pod_rotation = glm::rotate(glm::mat4(1.0f), (1.3f / 180.0f * glm::pi<float>()), glm::vec3(0.0f, 1.0f, 0.0f));//0.3f
	glm::mat4 arm_rotation = glm::rotate(glm::mat4(1.0f), (-0.8f / 180.0f * glm::pi<float>()), glm::vec3(0.0f, 1.0f, 0.0f));//-0.1f
	glm::mat4 level_rotation = glm::rotate(glm::mat4(1.0f), (0.2f / 180.0f * glm::pi<float>()), glm::vec3(0.0f, 1.0f, 0.0f));//0.05f

	/* Rotate the pods. */
	for (int i = 0; i < pods.size(); i++)
	{
		MatrixTransform * pod = pods[i];
		pod->M = pod->M*pod_rotation;
	}
	/* Rotate the arms */
	for (int i = 0; i < arms.size(); i++)
	{
		MatrixTransform * arm = arms[i];
		arm->M = arm_rotation*arm->M;
	}
	/* Rotate about the center */
	for (int i = 0; i < levels.size(); i++)
	{
		MatrixTransform * level = levels[i];
		level->M = level_rotation*level->M;
	}

}

/* Update the arms, beams, and middle to change it's length. */
void Cake::extension()
{
	/* Use the timer to get oscillating numbers. */
	double timer = glfwGetTime();
	float num_1 = (float)std::fmod(timer, 5.0);
	float num_2 = (float)std::fmod(timer, 4.0);
	float num_3 = (float)std::fmod(timer, 3.0);
	//Set to variables and update so it oscillates.
	if (num_1 > 2.5f) num_1 = 5.0f - num_1;
	if (num_2 > 2.0f) num_2 = 4.0f - num_2;
	if (num_3 > 1.5f) num_3 = 3.0f - num_3;
	//Set translation matrices for transformations.
	glm::mat4 movement_1 = glm::translate(glm::mat4(1.0f), glm::vec3((float)num_1, 0.0f, 0.0f));
	glm::mat4 movement_2 = glm::translate(glm::mat4(1.0f), glm::vec3((float)num_2, 0.0f, 0.0f));
	glm::mat4 movement_3 = glm::translate(glm::mat4(1.0f), glm::vec3((float)num_3, 0.0f, 0.0f));
	glm::mat4 movement_y_1 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, (float)num_1, 0.0f));
	glm::mat4 movement_y_2 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, (float)num_2, 0.0f));
	/* Extend the arms. */
	for (int i = 0; i < arms_middle.size(); i++)
	{
		MatrixTransform * arm = arms_middle[i];
		if (std::fmod(i, 5) == 0)
		{
			arm->M = movement_1;
		}
		else if (std::fmod(i, 5) == 1)
		{
			arm->M = movement_2;
		}
		else if (std::fmod(i, 5) == 2)
		{
			arm->M = movement_3;
		}
		else arm->M = movement_1;
	}
	/* Extend the beams */
	for (int i = 0; i < beams_middle.size(); i++)
	{
		MatrixTransform * beam = beams_middle[i];
		if (std::fmod(i, 3) == 0)
		{
			beam->M = movement_1;
		}
		else if (std::fmod(i, 4) == 1)
		{
			beam->M = movement_2;
		}
		else if (std::fmod(i, 5) == 2)
		{
			beam->M = movement_3;
		}
		else beam->M = movement_1;
	}
	/* Extend the root middle */
	for (int i = 0; i < roots_middle.size(); i++)
	{
		MatrixTransform * root_m = roots_middle[i];
		root_m->M = movement_y_1;
	}
	/* Extend the root bottom */
	for (int i = 0; i < roots_bottom.size(); i++)
	{
		MatrixTransform * root_b = roots_bottom[i];
		root_b->M = movement_y_2;
	}
}

