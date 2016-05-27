#include "Camera.h"

using namespace std;

/* Setup the camera. */
Camera::Camera()
{
	setupCamera();
}

/* Deconstructor to safely delete when finished. */
Camera::~Camera()
{

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

/* Setup default camera for the object (Assuming the object is at the center of the world). */
void Camera::setupCamera() {
	//Get 3 variables for the camera.
	this->camera.position = Window::camera_pos;
	this->camera.lookat = glm::vec3(0.0f, 0.0f, 0.0f);
	this->camera.up = glm::vec3(0.0f, 1.0f, 0.0f);
	//Set up the rest of the camera.
	this->camera.direction = glm::normalize(camera.position - camera.lookat);
	this->camera.right = glm::normalize(glm::cross(camera.up, camera.direction));
}

/* Update the camera if passed in camera components. Get Camera coordinates and values from the Window class. */
void Camera::updateCamera(glm::vec3 e, glm::vec3 d, glm::vec3 up)
{
	//Get 3 variables for the camera.
	this->camera.position = e;
	this->camera.lookat = d;
	this->camera.up = up;
	//Set up the rest of the camera.
	this->camera.direction = glm::normalize(camera.position - camera.lookat);
	this->camera.right = glm::normalize(glm::cross(camera.up, camera.direction));
}

/* Trackball mapping used to map coordinates in a sphere instead of window coordinates x and y. */
glm::vec3 Camera::trackBallMapping(glm::vec3 point)
{
	glm::vec3 trackball_p;    // Vector v is the synthesized 3D position of the mouse location on the trackball.
	float depth;     // this is the depth of the mouse location: the delta between the plane through the center of the trackball and the z position of the mouse.
	trackball_p.x = (float)((2.0*point.x - Window::width) / Window::width);   // this calculates the mouse X position in trackball coordinates, which range from -1 to +1.
	trackball_p.y = (float)((Window::height - 2.0*point.y) / Window::height);   // this does the equivalent to the above for the mouse Y position.
	trackball_p.z = 0.0;   // initially the mouse z position is set to zero, but this will change below.
	depth = (float)glm::length(trackball_p);    // this is the distance from the trackball's origin to the mouse location, without considering depth (=in the plane of the trackball's origin).
	depth = (float)((depth<1.0) ? depth : 1.0);   // this limits d to values of 1.0 or less to avoid square roots of negative values in the following line.
	trackball_p.z = (float)(sqrtf((float)1.001 - (float)(depth*depth)));  // this calculates the Z coordinate of the mouse position on the trackball, based on Pythagoras: v.z*v.z + d*d = 1*1.
	trackball_p = glm::normalize(trackball_p); // Still need to normalize, since we only capped d, not v.
	return trackball_p;  // return the mouse location on the surface of the trackball.
}

/* Rotate around the middle of the screen based on mouse drag from v to w. */
void Camera::camera_rotate(glm::vec3 v, glm::vec3 w)
{
	//Hold camera variables in vec4 for matrix multiplication.
	glm::vec4 pos = glm::vec4(this->camera.position, 1.0f);
	glm::vec4 up = glm::vec4(this->camera.up, 1.0f);

	glm::vec3 direction = w - v;
	float velocity = (float)glm::length(direction);
	if (velocity > 0.0001)
	{
		//Calculate Rotation Axis.
		glm::vec3 rotAxis = glm::cross(v, w);
		//Calculate Rotation Angle.
		float rot_angle = acos(glm::dot(v, w));
		//Calculate Rotation.
		glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), (rot_angle / 180.0f * glm::pi<float>()), rotAxis);
		//Calculate Camera.
		pos = rotate*pos;
		up = rotate*up;
		//Update camera.
		this->camera.position = glm::vec3(pos);
		this->camera.up = glm::vec3(up);
		//Update camera vectors.
		updateCamera(camera.position, camera.lookat, camera.up);
	}
}

/* Translate the object in camera space (x,y). */
void Camera::camera_translate(glm::vec3 v, glm::vec3 w)
{
	//Calculate the translation.
	glm::vec3 translate_v = glm::vec3(w.x - v.x, v.y - w.y, 0.0f);//Since x- and x+ are from left to right, y must be inverted so that y- and y+ are from bottom to top.
	translate_v *= (float)(1 / (float)Window::width);//Scale the translation.
													 //Calculate Camera.
	glm::vec3 position_x = this->camera.right*translate_v.x;//Moving left and right.
	glm::vec3 position_y = this->camera.up*translate_v.y;//Moving left and right.
														 //Moving along the xy plane.
	this->camera.position += position_x;
	this->camera.position += position_y;
	//Moving the lookat, to give it a strafing effect.
	this->camera.lookat += position_x;
	this->camera.lookat += position_y;
	//Update camera vectors.
	updateCamera(camera.position, camera.lookat, camera.up);
}

/* Translate the object in camera space (z). */
void Camera::zoom(double y)
{
	//Calculate the translation.
	//float z = (float)glm::clamp(y, -0.25, 0.25);//Translate only in the Z coordinate.
	float z = (float)y;
	//Calculate new camera distance.
	glm::vec3 direction = this->camera.direction;//position in relation to the origin.
	glm::vec3 displacement = direction * z;//Multiply Z so it moves in the correct direction (towards the origin).
	glm::vec3 zoom = this->camera.position - displacement;
	//Zoom is limited. Calculate threshold.
	if (glm::distance(zoom, camera.lookat) > 2)
	{
		this->camera.position = zoom;
	}
	//Update camera vectors.
	updateCamera(camera.position, camera.lookat, camera.up);
}

/* [Window Class] Update the camera, based on the object's camera struct. */
void Camera::window_updateCamera()
{
	Window::updateCamera(this->camera.position, this->camera.lookat, this->camera.up);
}












