#pragma once

#include "Window.h"
#include "Definitions.h"

#ifndef CAMERA_H
#define CAMERA_H
class Camera {
private:
	Global_Camera camera;

	OBJObject * toFollow;

	void setupCamera(glm::vec3 e, glm::vec3 d, glm::vec3 up);
	
	//Updates the corresponding camera vectors.
	void updateCamera();

public:
	Camera(glm::vec3 e, glm::vec3 d, glm::vec3 up);
	Camera(OBJObject * object_follow);
	~Camera();

	//Manipulate or change the camera components, if necessary.
	glm::vec3 get_cam_pos();
	glm::vec3 get_cam_look_at();
	glm::vec3 get_cam_up();
	glm::vec3 set_cam_pos(glm::vec3 update);
	glm::vec3 set_cam_look_at(glm::vec3 update);
	glm::vec3 set_cam_up(glm::vec3 update);

	//Functions for following an object.
	void camera_rotate_around(glm::vec3 v, glm::vec3 w);
	void camera_zoom(double y);
	void object_follow();

	//Functions for free world camera.
	void camera_translate(glm::vec3 v, glm::vec3 w);

	//Update the main window class with current camera coordinates.
	void window_updateCamera();

};
#endif