#include "Window.h"

const char* window_title = "GLFW Starter Project";
int Window::width;
int Window::height;

OBJObject object("test.obj");
OBJObject objf1("bunny.obj");
OBJObject objf2("abear.obj");
OBJObject objf3("adragon.obj");

void Window::initialize_objects()
{
}

void Window::clean_up()
{
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

	// Call the resize to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
	Window::width = width;
	Window::height = height;
	// Set the viewport size
	glViewport(0, 0, width, height);
	// Set the matrix mode to GL_PROJECTION to determine the proper camera properties
	glMatrixMode(GL_PROJECTION);
	// Load the identity matrix
	glLoadIdentity();
	// Set the perspective of the projection viewing frustum
	gluPerspective(60.0, double(width) / (double)height, 1.0, 1000.0);
	// Move camera back 20 units so that it looks at the origin (or else it's in the origin)
	glTranslatef(0, 0, -20);
}

void Window::idle_callback()
{
	// Perform any updates as necessary. Here, we will spin the cube slightly.
	object.update();
}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Set the matrix mode to GL_MODELVIEW
	glMatrixMode(GL_MODELVIEW);
	// Load the identity matrix
	glLoadIdentity();
	
	// Render objects
	object.draw();

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

	//Callback for 'x'/'X': move left/right by a small amount.
	if (pKey == GLFW_PRESS && (Lshift == GLFW_PRESS || Rshift == GLFW_PRESS)) object.pointUp();
	else if (pKey == GLFW_PRESS) object.pointDown();

	//Callback for 'x'/'X': move left/right by a small amount.
	if (xKey == GLFW_PRESS && (Lshift == GLFW_PRESS || Rshift == GLFW_PRESS)) object.right();
	else if (xKey == GLFW_PRESS) object.left();
	
	//Callback for 'y'/'Y': move down/up by a small amount.
	if (yKey == GLFW_PRESS && (Lshift == GLFW_PRESS || Rshift == GLFW_PRESS)) object.up();
	else if (yKey == GLFW_PRESS) object.down();
	
	//Callback for 'z'/'Z': move into/out of the screen by a small amount.
	if (zKey == GLFW_PRESS && (Lshift == GLFW_PRESS || Rshift == GLFW_PRESS)) object.out();
	else if (zKey == GLFW_PRESS) object.in();
	
	//Callback for 's'/'S': scale down/up (about the model's center, not the center of the screen).
	if (sKey == GLFW_PRESS && (Lshift == GLFW_PRESS || Rshift == GLFW_PRESS)) object.scaleUp();
	else if (sKey == GLFW_PRESS) object.scaleDown();
	
	//Callback for 'o'/'O': orbit the model about the window's z axis by a small number of degrees per key press,  
	//counterclockwise ('o') or clockwise ('O'). The z axis crosses the screen in the center of the window.
	if (oKey == GLFW_PRESS && (Lshift == GLFW_PRESS || Rshift == GLFW_PRESS)) object.orbitCW();
	else if (oKey == GLFW_PRESS) object.orbitCCW();

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
			object = objf1;//Load the first model
		}
		//Callback for F2
		if (key == GLFW_KEY_F2)
		{
			object = objf2;//Load the second model
		}
		//Callback for F3
		if (key == GLFW_KEY_F3)
		{
			object = objf3;//Load the third model
		}
		//Callback for 'r': reset position, orientation and size.
		if (key == GLFW_KEY_R) 
		{
			object.reset();//Reset the model
		}
	}
}