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

using namespace std;

const char* window_title = "CSE 167 Final";

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
Camera * world_camera;

//Define any objects here.
OBJObject * object_1;

//Define any environment variables here. We should always have the skybox!
SkyBox * skyBox;
Scenery * scenery;
Light * world_light;

//Define any shaders here.
GLint shaderProgram;
GLint shaderProgram_skybox;
GLint shaderProgram_terrain;

//Window properties
int Window::width;//Width of the window.
int Window::height;//Height of the window.
double Window::x;//Current mouse x coordinate.
double Window::y;//Current mouse y coordinate.
int Window::mouse_status = IDLE;//Define the mouse status for any clicks.
glm::vec3 Window::lastPoint;//Last point clicked.
int Window::camera_mode = CAMERA_WORLD;//Defined camera for controls.
glm::vec3 Window::camera_pos = glm::vec3(0.0f, 0.0f, 20.0f);//Default.
glm::vec3 Window::camera_look_at = glm::vec3(0.0f, 0.0f, 0.0f);//Default.
glm::vec3 Window::camera_up = glm::vec3(0.0f, 1.0f, 0.0f);//Default. 
glm::mat4 Window::P;//Perspective.
glm::mat4 Window::V;//View.

void Window::initialize_objects()
{
	//Initialize world variables.
	world_camera = new Camera(Window::camera_pos, Window::camera_look_at, Window::camera_up);//Initialize the global world camera.
	world_light = new Light();//Initialize the global light.
	skyBox = new SkyBox();//Initialize the default skybox.
	scenery = new Scenery(8, 8);

	//------------------------------ Windows (both 32 and 64 bit versions) ------------------------------ //
	#ifdef _WIN32 

	//Initialize any objects here, set it to a material.
	object_1 = new OBJObject("../obj/pod.obj", 1);

	//Initialize any terrains here.

	//Load the shader programs. Similar to the .obj objects, different platforms expect a different directory for files
	shaderProgram = LoadShaders("../shader.vert", "../shader.frag");
	shaderProgram_skybox = LoadShaders("../skybox.vert", "../skybox.frag");
	shaderProgram_terrain = LoadShaders("../terrain.vert", "../terrain.frag");
	
	//----------------------------------- Not Windows (MAC OSX) ---------------------------------------- //
	#else

	//Initialize any objects here, set it to a material.
	object_1 = new OBJObject("../obj/pod.obj", 1);

	//Initialize any terrains here.
	terrain = new Terrain(0, 0, "./terrain/texture_0.ppm", "./terrain/texture_1.ppm", "./terrain/texture_2.ppm", "./terrain/texture_3.ppm", "./terrain/blend_map.ppm", "./terrain/height_map.ppm", skyBox->getSkyBox());

	//Load the shader programs. Similar to the .obj objects, different platforms expect a different directory for files
	shaderProgram = LoadShaders("shader.vert", "shader.frag");
	shaderProgram_skybox = LoadShaders("skybox.vert", "skybox.frag");
	shaderProgram_terrain = LoadShaders("terrain.vert", "terrain.frag");

	#endif

	world_light->updateLighting(shaderProgram);
	world_light->updateLighting(shaderProgram_terrain);
}

void Window::clean_up()
{
	//Delete any instantiated objects.
	delete(world_camera);
	delete(world_light);
	delete(skyBox);
	delete(object_1);
	delete(scenery);
	//Delete shaders.
	glDeleteProgram(shaderProgram);
	glDeleteProgram(shaderProgram_skybox);
	glDeleteProgram(shaderProgram_terrain);
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

}

void Window::display_callback(GLFWwindow* window)
{
	//Draw the entire scene.
	Window::redrawScene();
	//Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	//Swap buffers
	glfwSwapBuffers(window);
}

void Window::redrawScene()
{
	//Clear the color and depth buffers
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Use the shader of programID
	glUseProgram(shaderProgram);
	//Render the objects
	object_1->draw(shaderProgram);

	//Use the shader of programID
	glUseProgram(shaderProgram_terrain);
	//Render the terrain
	scenery->draw(shaderProgram_terrain);

	//Use the shader of programID
	glUseProgram(shaderProgram_skybox);
	//Render the skybox
	skyBox->draw(shaderProgram_skybox);
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


	//Check for a single key press (Not holds)
	if (action == GLFW_PRESS)
	{
		//Check if escape was pressed.
		if (key == GLFW_KEY_ESCAPE)
		{
			//Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_T){

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
			world_camera->camera_rotate_around(Window::lastPoint, point);//Use this to orbit the camera.
			world_camera->window_updateCamera();
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
		world_camera->camera_zoom(yoffset);
		world_camera->window_updateCamera();
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
