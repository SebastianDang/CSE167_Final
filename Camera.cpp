#include "Camera.h"
#include "OBJObject.h"

using namespace std;

#define MAX_CAMERA_PITCH 40.0f//NEGATIVE ANGLE
#define ZOOM_MIN_DISTANCE 2.0f
#define ZOOM_MAX_DISTANCE 50.0f

/* Setup the camera. */
Camera::Camera(glm::vec3 e, glm::vec3 d, glm::vec3 up)
{
	setupCamera(e, d, up);
}

/* Deconstructor to safely delete when finished. */
Camera::~Camera()
{

}

/* Setup camera for the object. */
void Camera::setupCamera(glm::vec3 e, glm::vec3 d, glm::vec3 up) {
	//Get 3 variables for the camera.
	this->camera.position = e;
	this->camera.lookat = d;
	this->camera.up = up;
	//Set up the rest of the camera.
	this->camera.direction = glm::normalize(camera.position - camera.lookat);
	this->camera.right = glm::normalize(glm::cross(camera.up, camera.direction));
}

/* Returns the current camera's position. */
glm::vec3 Camera::get_cam_pos()
{
	return this->camera.position;
}

/* Returns the current camera's lookat vector. */
glm::vec3 Camera::get_cam_look_at()
{
	return this->camera.lookat;
}

/* Returns the current camera's up vector. */
glm::vec3 Camera::get_cam_up()
{
	return this->camera.up;
}

/* Updates the current camera's position. */
glm::vec3 Camera::set_cam_pos(glm::vec3 update)
{
	this->camera.position = update;
	updateCamera();
}

/* Updates the current camera's lookat vector. */
glm::vec3 Camera::set_cam_look_at(glm::vec3 update)
{
	this->camera.lookat = update;
	updateCamera();
}

/* Updates the current camera's up vector. */
glm::vec3 Camera::set_cam_up(glm::vec3 update)
{
	this->camera.up = update;
	updateCamera();
}

/* Update the camera variables. */
void Camera::updateCamera() {
	this->camera.direction = glm::normalize(camera.position - camera.lookat);
	this->camera.right = glm::normalize(glm::cross(camera.up, camera.direction));
}

/* Rotate around the middle of the screen based on mouse drag from v to w. */
void Camera::camera_rotate_around(glm::vec3 v, glm::vec3 w)
{
	//Calculate the translation.
	glm::vec3 translate_v = glm::vec3(w.x - v.x, v.y - w.y, 0.0f);//Since x- and x+ are from left to right, y must be inverted so that y- and y+ are from bottom to top.
	translate_v *= (float)(1 / (5*(float)Window::width));//Scale the translation.
	//Get the old position values for x.
	glm::vec3 cur_position_x = this->camera.position;
	float cur_position_length_x = glm::length(cur_position_x);
	//Calculate the new (x, z) position.
	glm::vec3 position_x = translate_v.x*this->camera.right;
	glm::vec3 new_position_x = this->camera.position + position_x;
	new_position_x = cur_position_length_x * glm::normalize(new_position_x);
	//Update the new camera position for x.
	this->camera.position = new_position_x;
	//Get the old position values for y.
	glm::vec3 cur_position_y = this->camera.position;
	float cur_position_length = glm::length(cur_position_y);
	//Calculate the new (y) position.
	glm::vec3 position_y = translate_v.y*this->camera.up;
	glm::vec3 new_position_y = this->camera.position + position_y;
	new_position_y = cur_position_length * glm::normalize(new_position_y);
	glm::vec3 cur_distance = new_position_y - this->camera.lookat;
	//Check to make sure it doesn't hit a max.
	glm::vec3 horizontal_pos = glm::vec3(this->camera.position.x, 0.0f, this->camera.position.z);
	glm::vec3 horizontal_lookat = glm::vec3(this->camera.lookat.x, 0.0f, this->camera.lookat.z);
	glm::vec3 horizontal_distance = horizontal_pos - horizontal_lookat;
	float angle = glm::dot(cur_distance, horizontal_distance) / (glm::length(cur_distance)*glm::length(horizontal_distance));
	//Update the new camera position for y. Check if the angle is greater than 0.0f and less than MAX_CAMERA_PITCH
	if (angle >= glm::radians(MAX_CAMERA_PITCH))
	{
		this->camera.position = new_position_y;
		if (this->camera.position.y <= 0.0f)
		{
			this->camera.position.y = 0.0f;
		}
	}
	//Update camera vectors.
	updateCamera();
}

/* Translate the object in camera space (z). */
void Camera::camera_zoom(double y)
{
	//Calculate the translation.
	float z = (float)y;
	//Calculate new camera distance.
	glm::vec3 direction = this->camera.direction;//position in relation to the origin.
	glm::vec3 displacement = direction * z;//Multiply Z so it moves in the correct direction (towards the origin).
	glm::vec3 zoom = this->camera.position - displacement;
	//Zoom is limited. Calculate threshold.
	if ((glm::distance(zoom, camera.lookat) > ZOOM_MIN_DISTANCE) && (glm::distance(zoom, camera.lookat) < ZOOM_MAX_DISTANCE))
	{
		this->camera.position = zoom;
	}
	//Update camera vectors.
	updateCamera();
}

/* [Window Class] Update the camera, based on the object's camera struct. */
void Camera::window_updateCamera()
{
	Window::updateCamera(this->camera.position, this->camera.lookat, this->camera.up);
}


