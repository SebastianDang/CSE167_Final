#pragma once

#include "Window.h"
#include "Definitions.h"

#ifndef CAMERA_H
#define CAMERA_H
class Camera {
private:
	Global_Camera camera;

	void setupCamera();
	void updateCamera(glm::vec3 e, glm::vec3 d, glm::vec3 up);


public:
	Camera();
	~Camera();

	glm::vec3 get_cam_pos();
	glm::vec3 get_cam_look_at();
	glm::vec3 get_cam_up();

	glm::vec3 trackBallMapping(glm::vec3 point);

	void camera_rotate(glm::vec3 v, glm::vec3 w);
	void camera_translate(glm::vec3 v, glm::vec3 w);
	void zoom(double y);

	void window_updateCamera();


};
#endif