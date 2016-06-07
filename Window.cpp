#include "window.h"
#include "group.h"
#include "Cylinder.h"
#include "Pod.h"
#include "Cake.h"
#include "Track.h"
#include "Terrain.h"
#include "Camera.h"
#include "Light.h"
#include "Scenery.h"
#include "Water.h"
#include "Particle.h"

using namespace std;

const char* window_title = "The Island";

//Define draw modes.
#define DRAW_MODE_ALL 0
#define DRAW_MODE_TERRAIN 1
#define DRAW_MODE_WATER 2
#define DRAW_MODE_PARTICLE 3
#define DRAW_MODE_COLLISION 4

//Define Mouse control status for idle, left hold, right hold.
#define IDLE 0
#define LEFT_HOLD 1
#define RIGHT_HOLD 2

//Define mode for controlling the object, or switching between cameras. We can define any additional camera views here.
#define CAMERA_WORLD 0
#define CAMERA_1 1
#define CAMERA_2 2
#define CAMERA_3 3

//Define any cameras here.
Camera * object_1_camera;
Camera * object_2_camera;

//Define any objects here.
OBJObject * object_1;
OBJObject * object_2;

//Define any environment variables here. We should always have the skybox!
SkyBox * skyBox;
Scenery * scenery;
Light * world_light;

//Define any shaders here.
GLint shaderProgram;
GLint shaderProgram_skybox;
GLint shaderProgram_terrain;
GLint shaderProgram_water;
GLint shaderProgram_particle;
GLint shaderProgram_collision;

//Window properties
int Window::width;//Width of the window.
int Window::height;//Height of the window.
double Window::x;//Current mouse x coordinate.
double Window::y;//Current mouse y coordinate.
int Window::mouse_status = IDLE;//Define the mouse status for any clicks.
glm::vec3 Window::lastPoint;//Last point clicked.
int Window::camera_mode = CAMERA_1;//Defined camera for controls.
glm::vec3 Window::camera_pos = glm::vec3(0.0f, 0.0f, 20.0f);//Default.
glm::vec3 Window::camera_look_at = glm::vec3(0.0f, 0.0f, 0.0f);//Default.
glm::vec3 Window::camera_up = glm::vec3(0.0f, 1.0f, 0.0f);//Default. 
glm::mat4 Window::P;//Perspective.
glm::mat4 Window::V;//View.
int Window::draw_mode = DRAW_MODE_ALL;

//Calculate frame time.
float Window::lastFrameTime;
float Window::delta;

//Toon shading boolean.
bool Window::toon_shading = false;

void Window::initialize_objects()
{
	//Initialize world variables.
	skyBox = new SkyBox();//Initialize the default skybox.
	scenery = new Scenery(1, 1, skyBox->getSkyBox());//Initialize the scenery for the entire program.
	world_light = new Light();//Initialize the global light.

	//------------------------------ Windows (both 32 and 64 bit versions) ------------------------------ //
	#ifdef _WIN32 

	//Initialize any objects here, set it to a material.
	object_1 = new OBJObject("../obj/songoku.obj", 5);
	object_1_camera = new Camera(object_1);

	object_2 = new OBJObject("../obj/pod.obj", 3);
	object_2_camera = new Camera(object_2);

	//Load the shader programs. Similar to the .obj objects, different platforms expect a different directory for files
	shaderProgram = LoadShaders("../shader.vert", "../shader.frag");
	shaderProgram_skybox = LoadShaders("../skybox.vert", "../skybox.frag");
	shaderProgram_terrain = LoadShaders("../terrain.vert", "../terrain.frag");
	shaderProgram_water = LoadShaders("../water.vert", "../water.frag");
	shaderProgram_particle = LoadShaders("../particle.vert", "../particle.frag");
	shaderProgram_collision = LoadShaders("../collision.vert", "../collision.frag");

	
	//----------------------------------- Not Windows (MAC OSX) ---------------------------------------- //
	#else

	//Initialize any objects here, set it to a material.
	object_1 = new OBJObject("./obj/songoku.obj", 5);
	object_1_camera = new Camera(object_1);

	//Load the shader programs. Similar to the .obj objects, different platforms expect a different directory for files
	shaderProgram = LoadShaders("./shader.vert", "./shader.frag");
	shaderProgram_skybox = LoadShaders("./skybox.vert", "./skybox.frag");
	shaderProgram_terrain = LoadShaders("./terrain.vert", "./terrain.frag");
	shaderProgram_water = LoadShaders("./water.vert", "./water.frag");
	shaderProgram_particle = LoadShaders("./particle.vert", "./particle.frag");

	#endif

	//Update the shader program and anything else.
	world_light->updateLighting(shaderProgram);
	object_1_camera->window_updateCamera();
}

void Window::clean_up()
{
	//Delete any instantiated objects.
	delete(skyBox);
	delete(scenery);
	delete(world_light);
	delete(object_1);
	delete(object_2);
	delete(object_1_camera);
	delete(object_2_camera);

	//Delete shaders.
	glDeleteProgram(shaderProgram);
	glDeleteProgram(shaderProgram_skybox);
	glDeleteProgram(shaderProgram_terrain);
	glDeleteProgram(shaderProgram_water);
	glDeleteProgram(shaderProgram_particle);
	glDeleteProgram(shaderProgram_collision);
}

GLFWwindow* Window::create_window(int width, int height)
{
	//Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	//4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

	//------------------------------ Windows (both 32 and 64 bit versions) ------------------------------ //
	#ifdef _WIN32

	//----------------------------------- Not Windows (MAC OSX) ---------------------------------------- //
	#else
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	//Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	//Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return NULL;
	}

	//Make the context of the window
	glfwMakeContextCurrent(window);

	//Set swap interval to 1
	glfwSwapInterval(1);

	//Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	//Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	//Set the initial frame time.
	Window::lastFrameTime = glfwGetTime();

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
	Window::width = width;
	Window::height = height;
	// Set the viewport size
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		V = glm::lookAt(Window::camera_pos, Window::camera_look_at, Window::camera_up);
	}
}

void Window::idle_callback()
{
	//Get the frame time.
	float currentFrameTime = glfwGetTime();
	Window::delta = (currentFrameTime - Window::lastFrameTime);
	Window::lastFrameTime = currentFrameTime;
	if (Window::toon_shading)
	{
		scenery->update_particles();
	}

	if (object_1->collision(object_2))
	{

	}
	if (Window::draw_mode == DRAW_MODE_COLLISION)
	{
		drawCollision();
	}
}

void Window::display_callback(GLFWwindow* window)
{
	if (Window::draw_mode == DRAW_MODE_ALL)
	{
		//Draw the entire scene.
		Window::redrawScene();
	}
	else if (Window::draw_mode == DRAW_MODE_TERRAIN)
	{
		Window::drawTerrain();
	}
	else if (Window::draw_mode == DRAW_MODE_WATER)
	{
		Window::drawWater();
	}
	else if (Window::draw_mode == DRAW_MODE_PARTICLE)
	{
		Window::drawParticles();
	}
	else if (Window::draw_mode == DRAW_MODE_COLLISION)
	{
		//Draw the entire scene.
		Window::redrawScene();
		Window::drawCollision();
	}
	//Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	//Swap buffers
	glfwSwapBuffers(window);
}

void Window::redrawScene()
{
	//Clear the color and depth buffers
	glClearColor(0.5f, 0.5f, 0.5f , 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Use the shader of programID
	glUseProgram(shaderProgram);
	//Render the objects
	object_1->draw(shaderProgram);
	object_2->draw(shaderProgram);

	//Use the shader of programID
	glUseProgram(shaderProgram_particle);
	//Render the objects
	scenery->draw_particles(shaderProgram_particle);

	//Use the shader of programID
	glUseProgram(shaderProgram_terrain);
	//Render the terrain
	scenery->draw_terrain(shaderProgram_terrain);

	//Use the shader of programID
	glUseProgram(shaderProgram_water);
	//Render the terrain
	scenery->draw_water(shaderProgram_water);

	//Use the shader of programID
	glUseProgram(shaderProgram_skybox);
	//Render the skybox
	skyBox->draw(shaderProgram_skybox);

}

void Window::drawTerrain()
{
	//Clear the color and depth buffers
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Use the shader of programID
	glUseProgram(shaderProgram);
	//Render the objects
	object_1->draw(shaderProgram);

	//Use the shader of programID
	glUseProgram(shaderProgram_terrain);
	//Render the terrain
	scenery->draw_terrain(shaderProgram_terrain);

	//Use the shader of programID
	glUseProgram(shaderProgram_skybox);
	//Render the skybox
	skyBox->draw(shaderProgram_skybox);
}

void Window::drawWater()
{
	//Clear the color and depth buffers
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Use the shader of programID
	glUseProgram(shaderProgram);
	//Render the objects
	object_1->draw(shaderProgram);

	//Use the shader of programID
	glUseProgram(shaderProgram_water);
	//Render the terrain
	scenery->draw_water(shaderProgram_water);

	//Use the shader of programID
	glUseProgram(shaderProgram_skybox);
	//Render the skybox
	skyBox->draw(shaderProgram_skybox);
}

void Window::drawParticles()
{
	//Clear the color and depth buffers
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Use the shader of programID
	glUseProgram(shaderProgram);
	//Render the objects
	object_1->draw(shaderProgram);

	//Use the shader of programID
	glUseProgram(shaderProgram_particle);
	//Render the objects
	scenery->draw_particles(shaderProgram_particle);

	//Use the shader of programID
	glUseProgram(shaderProgram_skybox);
	//Render the skybox
	skyBox->draw(shaderProgram_skybox);
}

void Window::drawCollision()
{
	//Use the shader of programID
	glUseProgram(shaderProgram_collision);
	//Render the skybox
	object_1->drawBox(shaderProgram_collision);
	object_2->drawBox(shaderProgram_collision);
}

/* Handle Key input. */
void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	/* Any global key definitions */
	//Define shift keys for capital letters.
	int Lshift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
	int Rshift = glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT);
	//Define movement keys.
	int wKey = glfwGetKey(window, GLFW_KEY_W);
	int aKey = glfwGetKey(window, GLFW_KEY_A);
	int sKey = glfwGetKey(window, GLFW_KEY_S);
	int dKey = glfwGetKey(window, GLFW_KEY_D);

	//Controls for the world camera.
	if (Window::camera_mode == CAMERA_WORLD)
	{

	}
	//Controls for camera 1.
	if (Window::camera_mode == CAMERA_1)
	{
		if (wKey == GLFW_PRESS)
		{
			object_1->W_movement(scenery->getBounds());
			object_1->update_height(scenery->getHeight(glm::vec3(object_1->toWorld[3])));
			object_1_camera->object_follow();
			object_1_camera->window_updateCamera();
		}
		if (aKey == GLFW_PRESS)
		{
			object_1->A_movement(scenery->getBounds());
			object_1->update_height(scenery->getHeight(glm::vec3(object_1->toWorld[3])));
			object_1_camera->object_follow();
			object_1_camera->window_updateCamera();
		}
		if (sKey == GLFW_PRESS)
		{
			object_1->S_movement(scenery->getBounds());
			object_1->update_height(scenery->getHeight(glm::vec3(object_1->toWorld[3])));
			object_1_camera->object_follow();
			object_1_camera->window_updateCamera();
		}
		if (dKey == GLFW_PRESS)
		{
			object_1->D_movement(scenery->getBounds());
			object_1->update_height(scenery->getHeight(glm::vec3(object_1->toWorld[3])));
			object_1_camera->object_follow();
			object_1_camera->window_updateCamera();
		}
	}
	//Controls for camera 2.
	if (Window::camera_mode == CAMERA_2)
	{
		if (wKey == GLFW_PRESS)
		{
			object_2->W_movement(scenery->getBounds());
			object_2->update_height(scenery->getHeight(glm::vec3(object_2->toWorld[3])));
			object_2_camera->object_follow();
			object_2_camera->window_updateCamera();
		}
		if (aKey == GLFW_PRESS)
		{
			object_2->A_movement(scenery->getBounds());
			object_2->update_height(scenery->getHeight(glm::vec3(object_2->toWorld[3])));
			object_2_camera->object_follow();
			object_2_camera->window_updateCamera();
		}
		if (sKey == GLFW_PRESS)
		{
			object_2->S_movement(scenery->getBounds());
			object_2->update_height(scenery->getHeight(glm::vec3(object_2->toWorld[3])));
			object_2_camera->object_follow();
			object_2_camera->window_updateCamera();
		}
		if (dKey == GLFW_PRESS)
		{
			object_2->D_movement(scenery->getBounds());
			object_2->update_height(scenery->getHeight(glm::vec3(object_2->toWorld[3])));
			object_2_camera->object_follow();
			object_2_camera->window_updateCamera();
		}
	}
	//---------- Anything below this will be global keys. ----------//
	//Check for a single key press (Not holds)
	if (action == GLFW_PRESS)
	{
		//Check if escape was pressed.
		if (key == GLFW_KEY_ESCAPE)
		{
			//Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_0){
			Window::draw_mode = DRAW_MODE_ALL;
		}
		if (key == GLFW_KEY_1) {
			Window::draw_mode = DRAW_MODE_TERRAIN;
		}
		if (key == GLFW_KEY_2) {
			Window::draw_mode = DRAW_MODE_WATER;
		}
		if (key == GLFW_KEY_3) {
			Window::draw_mode = DRAW_MODE_PARTICLE;
		}
		if (key == GLFW_KEY_4) {
			Window::draw_mode = DRAW_MODE_COLLISION;
		}
		if (key == GLFW_KEY_T) {
			scenery->toggleDrawMode();
		}
		if (key == GLFW_KEY_R) {
			if (Window::toon_shading)
			{
				Window::toon_shading = false;
			}
			else if (!Window::toon_shading)
			{
				Window::toon_shading = true;
			}
		}
		if (key == GLFW_KEY_C) {
			if (Window::camera_mode == CAMERA_1)
			{
				Window::camera_mode = CAMERA_2;
				object_2_camera->window_updateCamera();
			}
			else if (Window::camera_mode == CAMERA_2)
			{
				Window::camera_mode = CAMERA_1;
				object_1_camera->window_updateCamera();
			}
		}
	}
}

/* Handle mouse movement. */
void Window::cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	//Update current mouse position.
	glfwGetCursorPos(window, &xpos, &ypos);
	Window::x = xpos;
	Window::y = ypos;
	//Get current mouse position.
	glm::vec3 point = glm::vec3(Window::x, Window::y, 0.0f);
	//Controls for the world camera.
	if (Window::camera_mode == CAMERA_WORLD)
	{
		//On left drag, we perform rotations. Relative to the object.
		if (Window::mouse_status == LEFT_HOLD)
		{

		}
		//On right drag, we perform translations. Relative to the object.
		if (Window::mouse_status == RIGHT_HOLD)
		{

		}
	}
	//Controls for camera 1.
	if (Window::camera_mode == CAMERA_1)
	{
		//On left drag, we perform rotations. Relative to the object.
		if (Window::mouse_status == LEFT_HOLD)
		{
			object_1_camera->camera_rotate_around(Window::lastPoint, point);//Use this to orbit the camera.
			object_1_camera->window_updateCamera();
		}
		//On right drag, we perform translations. Relative to the object.
		if (Window::mouse_status == RIGHT_HOLD)
		{
		}
	}
	//Controls for camera 2.
	if (Window::camera_mode == CAMERA_2)
	{
		//On left drag, we perform rotations. Relative to the object.
		if (Window::mouse_status == LEFT_HOLD)
		{
			object_2_camera->camera_rotate_around(Window::lastPoint, point);//Use this to orbit the camera.
			object_2_camera->window_updateCamera();
		}
		//On right drag, we perform translations. Relative to the object.
		if (Window::mouse_status == RIGHT_HOLD)
		{
		}
	}
}

/* Handle mouse button input. Status handles if left button or right button was clicked and held. */
void Window::cursor_button_callback(GLFWwindow* window, int button, int action, int mods) 
{
	//Define left and right clicks.
	int left_click = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	int right_click = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	//Get current mouse position.
	glm::vec3 mouse_position = glm::vec3((float)Window::x, (float)Window::y, 0.0f);
	//Left click hold will save the position that the mouse was clicked and save it.
	if (left_click == GLFW_PRESS && right_click == GLFW_RELEASE && Window::mouse_status == IDLE) {
		Window::mouse_status = LEFT_HOLD;
		Window::lastPoint = mouse_position;
	}
	//Right click hold will save the position that the mouse was clicked and save it.
	else if (right_click == GLFW_PRESS && left_click == GLFW_RELEASE && Window::mouse_status == IDLE) {
		Window::mouse_status = RIGHT_HOLD;
		Window::lastPoint = mouse_position;
	}
	//If left click is held, then released, reset back to idle.
	else if (left_click == GLFW_RELEASE && Window::mouse_status == LEFT_HOLD) {
		Window::mouse_status = IDLE;
	}
	//If right click is held, then released, reset back to idle.
	else if (right_click == GLFW_RELEASE && Window::mouse_status == RIGHT_HOLD) {
		Window::mouse_status = IDLE;
	}
}

/* Handle mouse scroll input. */
void Window::cursor_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//Controls for the world camera.
	if (Window::camera_mode == CAMERA_WORLD)
	{

	}
	//Controls for camera 1.
	if (Window::camera_mode == CAMERA_1)
	{
		object_1_camera->camera_zoom(yoffset);
		object_1_camera->window_updateCamera();
	}
	//Controls for camera 1.
	if (Window::camera_mode == CAMERA_2)
	{
		object_2_camera->camera_zoom(yoffset);
		object_2_camera->window_updateCamera();
	}
}

/* Update the camera given e, d, and up vectors. We essentially rewrite the current camera. */
void Window::updateCamera(glm::vec3 e, glm::vec3 d, glm::vec3 up)
{
	Window::camera_pos = e;
	Window::camera_look_at = d;
	Window::camera_up = up;
	Window::V = glm::lookAt(e, d, up);
}
