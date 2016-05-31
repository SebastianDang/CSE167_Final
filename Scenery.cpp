#include "Scenery.h"
#include <string>
#include <string>
#include <sstream> 

#define TERRAIN_SIZE 500.0f

/* Constructor to create a terrain map with a specified width and height. */
Scenery::Scenery(int width, int height)
{
	this->width = width;
	this->height = height;
	generateTerrains();
	stitchTerrains();
}

/* Deconstructor to safely delete when finished. */
Scenery::~Scenery()
{

}

void Scenery::generateTerrains()
{
	//Generate terrains with width and height.
	for (int i = 0; i < this->height; i++)
	{
		for (int j = 0; j < this->width; j++)
		{
			std::string string_blend = "../terrain/blend_maps/blend_map_" + std::to_string((width*i) + j + 1) + ".ppm";
			const char* file_name_blend = string_blend.c_str();
			std::string string_height = "../terrain/height_maps/height_map_" + std::to_string((width*i) + j + 1) + ".ppm";
			const char* file_name_height = string_height.c_str();
			Terrain * cur_terrain = new Terrain(j, i, "../terrain/texture_0.ppm", "../terrain/texture_1.ppm", "../terrain/texture_2.ppm", "../terrain/texture_3.ppm", file_name_blend, file_name_height);
			terrains.push_back(cur_terrain);
		}
	}
}

/* Stitches the terrains based on the terrains array. */
void Scenery::stitchTerrains()
{
	//Stitch left and right.
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width - 1; j++)
		{
			terrains[(width*i) + j]->terrain_right = terrains[(width*i) + j + 1];
			terrains[(width*i) + j + 1]->terrain_left = terrains[(width*i) + j];
		}
	}
	//Stitch top and bottom.
	for (int j = 0; j < width; j++)
	{
		for (int i = 0; i < height-1; i++)
		{
			terrains[(width*i) + j]->terrain_bottom = terrains[(width*(i+1)) + j];
			terrains[(width*(i+1)) + j]->terrain_top = terrains[(width*i) + j];
		}
	}
	for (int x = 0; x < terrains.size(); x++)
	{
		terrains[x]->stitch_all();
	}
}

/* Calls draw on all the terrains. */
void Scenery::draw(GLuint shaderProgram)
{
	for (int i = 0; i < terrains.size(); i++)
	{
		terrains[i]->draw(shaderProgram);
	}
}

float Scenery::getTerrain(glm::vec3 position)
{
	float position_x = position.x / TERRAIN_SIZE;
	float position_z = position.z / TERRAIN_SIZE;
}
