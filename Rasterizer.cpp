#include <iostream>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "OBJObject.h"

#define DEBUG 0
#define MOVE_STEP 1.0f
#define ORBIT_STEP 5.0f
#define SPIN_STEP 1.0f
#define POINT_SIZE_STEP 1.0f
#define POINT_SIZE_MAX	10000.0f
#define POINT_SIZE_MIN	0.0f


static int window_width = 512, window_height = 512;
static float* pixels = new float[window_width * window_height * 3];

std::vector<glm::vec3> vertices;
std::vector<glm::vec3> normals;

glm::mat4 toWorld;
glm::mat4 c_inverse;
glm::mat4 projection;
glm::mat4 viewport;

float angle;
float orbitAngle;
float pointSize;

using namespace std;

struct Color    // generic color class
{
	float r, g, b;  // red, green, blue
};

glm::mat4 getWorld() {
	return toWorld;
}
glm::mat4 getCamera() {
	return c_inverse;
}
glm::mat4 getProjection() {
	return projection;
}
glm::mat4 getViewport() {
	return viewport;
}

void parse(const char *filepath) {
	std::FILE * objFile = fopen( filepath, "r");
	if (objFile == NULL) return;
	//Read the file until the end; "# are commments to be ignored".
	while (1) {
		char buf[BUFSIZ];
		int check = fscanf(objFile, "%s", buf);
		if (check == EOF) break;
		//Read in lines that start with "v". Add into vertices.
		if (strcmp(buf, "v") == 0) {
			glm::vec3 vertex;
			fscanf(objFile, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			//fscanf(objFile, "%f %f %f\n", ...colors...);//Colors?
			vertices.push_back(vertex);

		}
		//Read in lines that start with "vn". Add into normals.
		if (strcmp(buf, "vn") == 0) {
			glm::vec3 normalVertex;
			fscanf(objFile, "%f %f %f\n", &normalVertex.x, &normalVertex.y, &normalVertex.z);
			normals.push_back(normalVertex);
		}
		//Read in lines that start with "f". Add into faces.
	}
	fclose(objFile);
}

void loadData()
{
	parse("bunny.obj");
}

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

void spin(float deg)
{
	toWorld *= glm::rotate(glm::mat4(SPIN_STEP), 1.0f / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
}

void update()
{
	angle += SPIN_STEP;//Current Spin Angle.
	if (angle > 360.0f || angle < -360.0f) angle = 0.0f;
	//Spins.
	spin(angle);
}

void pointUp()
{
	printf("Rasterizer: P\n");
}
void pointDown()
{
	printf("Rasterizer: p\n");
}
void left()
{
	printf("Rasterizer: x\n");
}
void right()
{
	printf("Rasterizer: X\n");
}
void up()
{
	printf("Rasterizer: Y\n");
}
void down()
{
	printf("Rasterizer: y\n");
}
void in()
{
	printf("Rasterizer: z\n");
}
void out()
{
	printf("Rasterizer: Z\n");
}
void scaleUp()
{
	printf("Rasterizer: S\n");
}
void scaleDown()
{
	printf("Rasterizer: s\n");
}
void orbitCW()
{
	printf("Rasterizer: O\n");
}
void orbitCCW()
{
	printf("Rasterizer: o\n");
}
void reset()
{
	printf("Rasterizer: r\n");
}

void rasterize()//Basically a new draw function.
{
	// Put your main rasterization loop here
	// It should go over the point model and call drawPoint for every point in it
	//update();

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
		if (DEBUG) printf("%f, %f, %f\n", point.x, point.y, point.z);

		//p' = M * p
		glm::mat4 world = getWorld();
		point = world * point;
		if (DEBUG) printf("%f, %f, %f\n", point.x, point.y, point.z);

		//p' = C^(-1) * M * p
		glm::mat4 camera = getCamera();
		point = camera * point;
		if (DEBUG) printf("%f, %f, %f\n", point.x, point.y, point.z);

		//p' = P * C^(-1) * M * p
		glm::mat4 projection = getProjection();
		point = projection * point;
		if (DEBUG) printf("%f, %f, %f\n", point.x, point.y, point.z);

		//p' = D * P * C^(-1) * M * p
		glm::mat4 viewport = getViewport();
		point = point*viewport;
		if (DEBUG) printf("%f, %f, %f\n", point.x, point.y, point.z);

		int dx = (int)(point.x/point.w);
		int dy = (int)(point.y/point.w);

		if (DEBUG) printf("%d, %d \n", dx, dy);
		
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
	if (pKey == GLFW_PRESS && (Lshift == GLFW_PRESS || Rshift == GLFW_PRESS)) pointUp();
	else if (pKey == GLFW_PRESS) pointDown();

	//Callback for 'x'/'X': move left/right by a small amount.
	if (xKey == GLFW_PRESS && (Lshift == GLFW_PRESS || Rshift == GLFW_PRESS)) right();
	else if (xKey == GLFW_PRESS) left();

	//Callback for 'y'/'Y': move down/up by a small amount.
	if (yKey == GLFW_PRESS && (Lshift == GLFW_PRESS || Rshift == GLFW_PRESS)) up();
	else if (yKey == GLFW_PRESS) down();

	//Callback for 'z'/'Z': move into/out of the screen by a small amount.
	if (zKey == GLFW_PRESS && (Lshift == GLFW_PRESS || Rshift == GLFW_PRESS)) out();
	else if (zKey == GLFW_PRESS) in();

	//Callback for 's'/'S': scale down/up (about the model's center, not the center of the screen).
	if (sKey == GLFW_PRESS && (Lshift == GLFW_PRESS || Rshift == GLFW_PRESS)) scaleUp();
	else if (sKey == GLFW_PRESS) scaleDown();

	//Callback for 'o'/'O': orbit the model about the window's z axis by a small number of degrees per key press,  
	//counterclockwise ('o') or clockwise ('O'). The z axis crosses the screen in the center of the window.
	if (oKey == GLFW_PRESS && (Lshift == GLFW_PRESS || Rshift == GLFW_PRESS)) orbitCW();
	else if (oKey == GLFW_PRESS) orbitCCW();

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
			//object = objf1;//Load the first model
		}
		//Callback for F2
		if (key == GLFW_KEY_F2)
		{
			//object = objf2;//Load the second model
		}
		//Callback for F3
		if (key == GLFW_KEY_F3)
		{
			//object = objf3;//Load the third model
		}
		//Callback for 'r': reset position, orientation and size.
		if (key == GLFW_KEY_R)
		{
			reset();//Reset the model
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

int main(int argc, char** argv) {

	//Initialize all matrices and local variables.
	angle = 0.0f;
	orbitAngle = 0.0f;
	pointSize = 1.0f;

	//toWorld Matrix
	toWorld = glm::mat4(1.0f);

	//C_inverse Matrix
	glm::vec3 e = glm::vec3(0.0f, 0.0f, 20.0f);
	glm::vec3 d = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	c_inverse = glm::lookAt(e, d, up);
	/*
	printf("c_inverse:\n %f, %f, %f, %f \n %f, %f, %f, %f \n %f, %f, %f, %f \n %f, %f, %f, %f \n", 
		c_inverse[0][0], c_inverse[1][0], c_inverse[2][0], c_inverse[3][0], 
		c_inverse[0][1], c_inverse[1][1], c_inverse[2][1], c_inverse[3][1], 
		c_inverse[0][2], c_inverse[1][2], c_inverse[2][2], c_inverse[3][2], 
		c_inverse[0][3], c_inverse[1][3], c_inverse[2][3], c_inverse[3][3]);
	*/

	//Projection Matrix
	projection = glm::perspective(glm::radians(60.0f), (float)window_width / (float)window_height, 1.0f, 1000.0f);
	/*
	printf("projection:\n %f, %f, %f, %f \n %f, %f, %f, %f \n %f, %f, %f, %f \n %f, %f, %f, %f \n",
		projection[0][0], projection[1][0], projection[2][0], projection[3][0],
		projection[0][1], projection[1][1], projection[2][1], projection[3][1],
		projection[0][2], projection[1][2], projection[2][2], projection[3][2],
		projection[0][3], projection[1][3], projection[2][3], projection[3][3]);
	*/

	//Viewport Matrix
	float viewportX = (window_width - 0.0f) * (0.5f);
	float viewportY = (window_height - 0.0f) * (0.5f);
	float viewportX2 = (window_width + 0.0f) * (0.5f);
	float viewportY2 = (window_height + 0.0f) * (0.5f);
	viewport = glm::mat4(viewportX, 0.0f, 0.0f, viewportX2,
		0.0f, viewportY, 0.0f, viewportY2,
		0.0f, 0.0f, 0.5f, 0.5f,
		0.0f, 0.0f, 0.0f, 1.0f);

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
	
	//Load the data.
	loadData();

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
