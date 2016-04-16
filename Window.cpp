#include "window.h"

const char* window_title = "CSE 167 Homework 2";

#define IDLE 0
#define LEFT_HOLD 1
#define RIGHT_HOLD 2

//Default Objects
OBJObject * object;
OBJObject * objectF1;
OBJObject * objectF2;
OBJObject * objectF3;

//Default Shader 
GLint shaderProgram;

//Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 20.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

//Window properties
int Window::width;
int Window::height;
double Window::x;
double Window::y;
int Window::status;
glm::vec3 Window::lastPoint;
glm::mat4 Window::P;
glm::mat4 Window::V;

void Window::initialize_objects()
{
	Window::status = IDLE;
	objectF1 = new OBJObject("bunny.obj");
	objectF2 = new OBJObject("abear.obj");
	objectF3 = new OBJObject("adragon.obj");
	object = objectF1;//Default with the bunny.
	// Load the shader program. Similar to the .obj objects, different platforms expect a different directory for files
	#ifdef _WIN32 // Windows (both 32 and 64 bit versions)
	shaderProgram = LoadShaders("../shader.vert", "../shader.frag");
	#else // Not windows
	shaderProgram = LoadShaders("shader.vert", "shader.frag");
	#endif
}

void Window::clean_up()
{
	delete(objectF1);
	delete(objectF2);
	delete(objectF3);
	glDeleteProgram(shaderProgram);
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
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{

}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use the shader of programID
	glUseProgram(shaderProgram);
	
	// Render the cube
	//cube->draw(shaderProgram);
	object->draw(shaderProgram);

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

/* Handle Key input. */
void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//Define shift keys for capital letters.
	int Lshift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
	int Rshift = glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT);
	//Define 's' key.
	int sKey = glfwGetKey(window, GLFW_KEY_S);
	//Callback for 's'/'S': scale down/up (about the model's center, not the center of the screen).
	if (sKey == GLFW_PRESS && (Lshift == GLFW_PRESS || Rshift == GLFW_PRESS)) object->scaleUp();
	else if (sKey == GLFW_PRESS) object->scaleDown();
	//Check for a single key press (Not holds)
	if (action == GLFW_PRESS)
	{
		//Check if escape was pressed.
		if (key == GLFW_KEY_ESCAPE)
		{
			//Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		//Check for F1.
		if (key == GLFW_KEY_F1)
		{
			object = objectF1;
		}
		//Check for F2.
		if (key == GLFW_KEY_F2)
		{
			object = objectF2;
		}
		//Check for F3.
		if (key == GLFW_KEY_F3)
		{
			object = objectF3;
		}
		//Check for 'r'.
		if (key == GLFW_KEY_R)
		{
			object->reset();
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
	//On left drag, we perform rotations.
	if (Window::status == LEFT_HOLD) 
	{
		glm::vec3 curPoint = object->trackBallMapping(point);
		object->rotate(Window::lastPoint, curPoint);
	}
	//On right drag, we perform translations.
	if (Window::status == RIGHT_HOLD) {

	}
}

/* Handle mouse button input. */
void Window::cursor_button_callback(GLFWwindow* window, int button, int action, int mods) 
{
	//Define left and right clicks.
	int left_click = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	int right_click = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	//Get current mouse position.
	glm::vec3 mouse_position = glm::vec3((float)Window::x, (float)Window::y, 0.0f);
	//Check for clicks and holds.
	if (left_click == GLFW_PRESS && right_click == GLFW_RELEASE && Window::status == IDLE) {
		Window::status = LEFT_HOLD;
		Window::lastPoint = object->trackBallMapping(mouse_position);
	}
	else if (right_click == GLFW_PRESS && left_click == GLFW_RELEASE && Window::status == IDLE) {
		Window::status = RIGHT_HOLD;
	}
	else if (left_click == GLFW_RELEASE && Window::status == LEFT_HOLD) {
		Window::status = IDLE;
	}
	else if (right_click == GLFW_RELEASE && Window::status == RIGHT_HOLD) {
		Window::status = IDLE;
	}
}