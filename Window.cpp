#include "window.h"

const char* window_title = "CSE 167 Homework 2";
OBJObject * object;
GLint shaderProgram;

// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 20.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

int Window::width;
int Window::height;

glm::mat4 Window::P;
glm::mat4 Window::V;

void Window::initialize_objects()
{
	object = new OBJObject("bunny.obj");
	// Load the shader program. Similar to the .obj objects, different platforms expect a different directory for files
	#ifdef _WIN32 // Windows (both 32 and 64 bit versions)
	shaderProgram = LoadShaders("../shader.vert", "../shader.frag");
	#else // Not windows
	shaderProgram = LoadShaders("shader.vert", "shader.frag");
	#endif
}

void Window::clean_up()
{
	delete(object);
	glDeleteProgram(shaderProgram);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
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
	object->update();
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

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//Define shift keys for capital letters
	int Lshift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
	int Rshift = glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT);

	//Define x, y, z, s, o keys
	int xKey = glfwGetKey(window, GLFW_KEY_X);
	int yKey = glfwGetKey(window, GLFW_KEY_Y);
	int zKey = glfwGetKey(window, GLFW_KEY_Z);
	int sKey = glfwGetKey(window, GLFW_KEY_S);
	int oKey = glfwGetKey(window, GLFW_KEY_O);
	int pKey = glfwGetKey(window, GLFW_KEY_P);

	//Callback for 'p'/'P': adjust point size by a small amount.
	if (pKey == GLFW_PRESS && (Lshift == GLFW_PRESS || Rshift == GLFW_PRESS)) object->pointUp();
	else if (pKey == GLFW_PRESS) object->pointDown();

	//Callback for 'x'/'X': move left/right by a small amount.
	if (xKey == GLFW_PRESS && (Lshift == GLFW_PRESS || Rshift == GLFW_PRESS)) object->right();
	else if (xKey == GLFW_PRESS) object->left();

	//Callback for 'y'/'Y': move down/up by a small amount.
	if (yKey == GLFW_PRESS && (Lshift == GLFW_PRESS || Rshift == GLFW_PRESS)) object->up();
	else if (yKey == GLFW_PRESS) object->down();

	//Callback for 'z'/'Z': move into/out of the screen by a small amount.
	if (zKey == GLFW_PRESS && (Lshift == GLFW_PRESS || Rshift == GLFW_PRESS)) object->out();
	else if (zKey == GLFW_PRESS) object->in();

	//Callback for 's'/'S': scale down/up (about the model's center, not the center of the screen).
	if (sKey == GLFW_PRESS && (Lshift == GLFW_PRESS || Rshift == GLFW_PRESS)) object->scaleUp();
	else if (sKey == GLFW_PRESS) object->scaleDown();

	//Callback for 'o'/'O': orbit the model about the window's z axis by a small number of degrees per key press,  
	//Counterclockwise ('o') or clockwise ('O'). The z axis crosses the screen in the center of the window.
	if (oKey == GLFW_PRESS && (Lshift == GLFW_PRESS || Rshift == GLFW_PRESS)) object->orbitCW();
	else if (oKey == GLFW_PRESS) object->orbitCCW();

	//Check for a single key press (Not holds)
	if (action == GLFW_PRESS)
	{
		// Check if escape was pressed
		if (key == GLFW_KEY_ESCAPE)
		{
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		//Callback for F1
		if (key == GLFW_KEY_F1)
		{
			OBJObject * objectHold = object;
			delete(objectHold);
			object = new OBJObject("bunny.obj");
		}
		//Callback for F2
		if (key == GLFW_KEY_F2)
		{
			OBJObject * objectHold = object;
			delete(objectHold);
			object = new OBJObject("bear.obj");
		}
		//Callback for F3
		if (key == GLFW_KEY_F3)
		{
			OBJObject * objectHold = object;
			delete(objectHold);
			object = new OBJObject("dragon.obj");
		}
	}
}