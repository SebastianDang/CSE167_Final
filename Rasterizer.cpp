#include <iostream>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "OBJObject.h"

static int window_width = 512, window_height = 512;
static float* pixels = new float[window_width * window_height * 3];

OBJObject rastObject("bunny.obj");

using namespace std;

struct Color    // generic color class
{
	float r, g, b;  // red, green, blue
};

// Clear frame buffer to the color black (0.0, 0.0, 0.0)
void clearBuffer()
{
	Color clearColor = { 0.0, 0.0, 0.0 };   // clear color: black
	for (int i = 0; i<window_width*window_height; ++i)
	{
		pixels[i * 3] = clearColor.r;
		pixels[i * 3 + 1] = clearColor.g;
		pixels[i * 3 + 2] = clearColor.b;
	}
}

// Draw a point into the frame buffer (x,y) (r,g,b)
void drawPoint(int x, int y, float r, float g, float b)
{
	int offset = y*window_width * 3 + x * 3;
	pixels[offset] = r;
	pixels[offset + 1] = g;
	pixels[offset + 2] = b;
}

void rasterize()//Basically a new draw function.
{
	std::vector<glm::vec3> vertices = rastObject.getVertices();
	std::vector<glm::vec3> normals = rastObject.getNormals();

	for (int i = 0; i < vertices.size(); ++i)
	{
		//Set color of the pixel
		Color objColor;
		glm::vec3 normalized = glm::normalize(normals[i]);//Normalize for the colors.
		float red = normalized.x;
		float green = normalized.y;
		float blue = normalized.z;

		if (red < 0.0f) { red = 0.0f + (-0.5f)*red; }
		else red = 0.0f + (0.5f)*red;

		if (green < 0.0f) { green = 0.0f + (-0.5f)*green; }
		else green = 0.0f + (0.5f)*green;

		if (blue < 0.0f) { blue = 0.0f + (-0.5f)*blue; }
		else blue = 0.0f + (0.5f)*blue;

		objColor.r = red;
		objColor.g = green;
		objColor.b = blue;

		//p' = p
		glm::vec4 point = glm::vec4(vertices[i].x, vertices[i].y, vertices[i].z, 1);

		//p' = M * p
		glm::mat4 world = rastObject.getWorld();

		//p' = C^(-1) * M * p
		glm::mat4 camera = rastObject.getCamera();

		//p' = P * C^(-1) * M * p
		glm::mat4 projection = rastObject.getProjection();
		point = projection * camera * world * point;

		//p' = D * P * C^(-1) * M * p
		glm::mat4 viewport = rastObject.getViewport();
		point = point*viewport;
		
		int dx = (int)(point.x/point.w);
		int dy = (int)(point.y/point.w);


		
		if (dx >= 0 && dx < window_width && dy >= 0 && dy < window_height)
		{
			drawPoint(dx, dy, objColor.r, objColor.g, objColor.b);
		}

	}

}

// Called whenever the window size changes
void resizeCallback(GLFWwindow* window, int width, int height)
{
	window_width = width;
	window_height = height;
	delete[] pixels;
	pixels = new float[window_width * window_height * 3];
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
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
	if (pKey == GLFW_PRESS && (Lshift == GLFW_PRESS || Rshift == GLFW_PRESS)) rastObject.pointUp();
	else if (pKey == GLFW_PRESS) rastObject.pointDown();

	//Callback for 'x'/'X': move left/right by a small amount.
	if (xKey == GLFW_PRESS && (Lshift == GLFW_PRESS || Rshift == GLFW_PRESS)) rastObject.right();
	else if (xKey == GLFW_PRESS) rastObject.left();

	//Callback for 'y'/'Y': move down/up by a small amount.
	if (yKey == GLFW_PRESS && (Lshift == GLFW_PRESS || Rshift == GLFW_PRESS)) rastObject.up();
	else if (yKey == GLFW_PRESS) rastObject.down();

	//Callback for 'z'/'Z': move into/out of the screen by a small amount.
	if (zKey == GLFW_PRESS && (Lshift == GLFW_PRESS || Rshift == GLFW_PRESS)) rastObject.out();
	else if (zKey == GLFW_PRESS) rastObject.in();

	//Callback for 's'/'S': scale down/up (about the model's center, not the center of the screen).
	if (sKey == GLFW_PRESS && (Lshift == GLFW_PRESS || Rshift == GLFW_PRESS)) rastObject.scaleUp();
	else if (sKey == GLFW_PRESS) rastObject.scaleDown();

	//Callback for 'o'/'O': orbit the model about the window's z axis by a small number of degrees per key press,  
	//counterclockwise ('o') or clockwise ('O'). The z axis crosses the screen in the center of the window.
	if (oKey == GLFW_PRESS && (Lshift == GLFW_PRESS || Rshift == GLFW_PRESS)) rastObject.orbitCW();
	else if (oKey == GLFW_PRESS) rastObject.orbitCCW();

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
			//rastObject = objf1;//Load the first model
		}
		//Callback for F2
		if (key == GLFW_KEY_F2)
		{
			//rastObject = objf2;//Load the second model
		}
		//Callback for F3
		if (key == GLFW_KEY_F3)
		{
			//rastObject = objf3;//Load the third model
		}
		//Callback for 'r': reset position, orientation and size.
		if (key == GLFW_KEY_R)
		{
			rastObject.reset();//Reset the model
		}
	}
}

void displayCallback(GLFWwindow* window)
{
	clearBuffer();
	rasterize();

	// glDrawPixels writes a block of pixels to the framebuffer
	glDrawPixels(window_width, window_height, GL_RGB, GL_FLOAT, pixels);

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void errorCallback(int error, const char* description)
{
	// Print error
	fputs(description, stderr);
}

int main2(int argc, char** argv) {

	//Projection Matrix
	rastObject.setProjection((float)window_width, (float)window_height);
	//Viewport Matrix
	rastObject.setViewport((float)window_width, (float)window_height);

	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(window_height, window_height, "Rasterizer", NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return -1;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Set the error callback
	glfwSetErrorCallback(errorCallback);
	// Set the key callback
	glfwSetKeyCallback(window, keyCallback);
	// Set the window resize callback
	glfwSetWindowSizeCallback(window, resizeCallback);

	// Loop while GLFW window should stay open
	while (!glfwWindowShouldClose(window))
	{
		// Main render display callback. Rendering of objects is done here.
		displayCallback(window);
	}

	// Destroy the window
	glfwDestroyWindow(window);
	// Terminate GLFW
	glfwTerminate();

	exit(EXIT_SUCCESS);
}
