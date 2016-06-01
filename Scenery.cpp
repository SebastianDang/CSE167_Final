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
			const char* file_names_blend = string_blend.c_str();
			std::string string_height = "../terrain/height_maps/height_map_" + std::to_string((width*i) + j + 1) + ".ppm";
			const char* file_names_height = string_height.c_str();
			Terrain * cur_terrain = new Terrain(j, i, "../terrain/texture_0.ppm", "../terrain/texture_1.ppm", "../terrain/texture_2.ppm", "../terrain/texture_3.ppm", file_names_blend, file_names_height);
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

/* Toggles the draw mode for wireframe mode or fill mode. */
void Scenery::toggleDrawMode()
{
	for (int i = 0; i < terrains.size(); i++)
	{
		terrains[i]->toggleDrawMode();
	}
}


/* Return the terrain number that the object is currently in. */
int Scenery::getTerrain(glm::vec3 position)
{
	float position_x = position.x / TERRAIN_SIZE;
	float position_z = position.z / TERRAIN_SIZE;

	if (position_x >= width || position_x < 0 || position_z >= height || position_z < 0) {
		printf("Accessing a point out of bounds of this Scenery. Please check to make sure we're accessing the proper coordinates.\n");
		return 0;
	}

	int terrain_x = floor(position_x);
	int terrain_z = floor(position_z);

	return (terrain_z*width + terrain_x);
}

/* Return the height for the given terrain (given position in the world). */
float Scenery::getHeight(glm::vec3 position)
{
	Terrain * terrain = terrains[getTerrain(position)];
	return terrain->getHeight(position);
}
