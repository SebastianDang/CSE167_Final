#ifndef WINDOW_H
#define WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <vector>

#include "shader.h"
#include "OBJObject.h"
#include "SkyBox.h"

class Window
{
private:
public:

	static double x;
	static double y;
	static glm::vec3 lastPoint;
	static int mouse_status;
	static int camera_mode;
	static int width;
	static int height;
	static glm::mat4 P;
	static glm::mat4 V;
	static glm::vec3 camera_pos;
	static glm::vec3 camera_look_at;
	static glm::vec3 camera_up;
	static void initialize_objects();
	static void clean_up();
	static GLFWwindow* create_window(int width, int height);
	static void resize_callback(GLFWwindow* window, int width, int height);
	static void idle_callback();
	static void display_callback(GLFWwindow*);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
	static void cursor_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void cursor_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void updateCamera(glm::vec3 e, glm::vec3 d, glm::vec3 up);
	static void redrawScene();

	static float lastFrameTime;
	static float delta;

	static bool toon_shading;

	//Seperated drawing for demo.
	static void drawTerrain();
	static void drawWater();
	static void drawParticles();
	static void drawCollision();

	static int draw_mode;

};

#endif
