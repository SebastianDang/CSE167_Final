#include "Terrain.h"
#include <time.h>

using namespace std;

#define SIZE 50
#define VERTEX_COUNT 128
#define MAX_HEIGHT 50
#define MAX_DISPLACEMENT 0.5f
#define DRAW_SHADED 0
#define DRAW_WIREFRAME 1

/* Flat Terrain. Ability to input a height map: either real or generated from different applications. Shader that adds at least 3 different type of terrain(grass, desert, snow). */
Terrain::Terrain(float x_d, float z_d, const char* terrain_0, const char* terrain_1, const char* terrain_2, const char* terrain_3, const char* blend_map)
{
	//Setup the terrain.
	this->x = x_d * SIZE;
	this->z = z_d * SIZE;
	this->draw_mode = DRAW_SHADED;
	//Setup toWorld so that the terrain is at the center of the world.
	this->toWorld = glm::mat4(1.0f);
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(this->x, 0, this->z));
	this->toWorld = translate*this->toWorld;
	//Setup HeightMap
	this->setupHeightMap();
	//Load the texture and setup VAO, VBO for the terrains.
	this->setupTerrain(terrain_0, terrain_1, terrain_2, terrain_3, blend_map);
}

/* Procedurally generated Terrain. Ability to input a height map: either real or generated from different applications. Shader that adds at least 3 different type of terrain(grass, desert, snow). */
Terrain::Terrain(float x_d, float z_d, const char* terrain_0, const char* terrain_1, const char* terrain_2, const char* terrain_3, const char* blend_map, const char* height_map)
{
	//Setup the terrain.
	this->x = x_d * SIZE;
	this->z = z_d * SIZE;
	this->draw_mode = DRAW_SHADED;
	//Setup toWorld so that the terrain is at the center of the world.
	this->toWorld = glm::mat4(1.0f);
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(this->x, 0, this->z));
	this->toWorld = translate*this->toWorld;
	//Setup HeightMap
	this->setupHeightMap(height_map, 128.0f, 4.0f);
	//Load the texture and setup VAO, VBO for the terrains.
	this->setupTerrain(terrain_0, terrain_1, terrain_2, terrain_3, blend_map);
}

/* Deconstructor to safely delete when finished. */
Terrain::~Terrain()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

/* Setup a default flat terrain. */
void Terrain::setupHeightMap()
{
	//Create the height map: v, vn, texCoords
	//Generate vertices, normals, and texCoords for a terrain map. vertex = (j, i).
	for (int i = 0; i < VERTEX_COUNT; i++)
	{
		for (int j = 0; j < VERTEX_COUNT; j++)
		{
			//Setup the vertices.
			float vertex_x = (float)j / ((float)VERTEX_COUNT - 1) * SIZE;
			float vertex_y = 0.0f;
			float vertex_z = (float)i / ((float)VERTEX_COUNT - 1) * SIZE;
			//Setup the normals.
			float normal_x = 0;
			float normal_y = 1.0f;
			float normal_z = 0;
			//Setup the texcoords.
			float texCoord_x = (float)j / ((float)VERTEX_COUNT - 1);
			float texCoord_y = (float)i / ((float)VERTEX_COUNT - 1);
			//Push back to vectors.
			vertices.push_back(glm::vec3(vertex_x, vertex_y, vertex_z));
			normals.push_back(glm::vec3(normal_x, normal_y, normal_z));
			texCoords.push_back(glm::vec2(texCoord_x, texCoord_y));
		}
	}
	//Setup the indices to draw based on indice points.
	for (int gz = 0; gz < VERTEX_COUNT - 1; gz++)
	{
		for (int gx = 0; gx < VERTEX_COUNT - 1; gx++)
		{
			int topLeft = (gz*VERTEX_COUNT) + gx;//0//1
			int topRight = topLeft + 1;//1//2
			int bottomLeft = ((gz + 1)*VERTEX_COUNT) + gx;//128//129
			int bottomRight = bottomLeft + 1;//129//130
			//Push back to indices.
			indices.push_back(topLeft);
			indices.push_back(bottomLeft);
			indices.push_back(topRight);
			indices.push_back(topRight);
			indices.push_back(bottomLeft);
			indices.push_back(bottomRight);
		}
	}
	//Add into container structs for rendering.
	for (int i = 0; i < VERTEX_COUNT * VERTEX_COUNT; i++)
	{
		//Throw everything into a container to hold all values.
		Container container;
		container.vertex = vertices[i];
		container.normal = normals[i];
		container.texCoord = texCoords[i];
		containers.push_back(container);
	}
}

/* Setup the terrain based on loaded height map. */
void Terrain::setupHeightMap(const char* filename, float n_smooth, float n_range)
{
	//Define variables to hold height map's width, height, pixel information.
	int width, height;
	unsigned char * image;
	//Generate the texture.
	image = loadPPM(filename, width, height);//Load the ppm file.
	//Create the height map: v, vn, texCoords
	//Generate vertices, normals, and texCoords for a terrain map.
	for (int i = 0; i < VERTEX_COUNT; i++)
	{
		for (int j = 0; j < VERTEX_COUNT; j++)
		{
			//Setup the vertices.
			float vertex_x = (float)j / ((float)VERTEX_COUNT - 1) * SIZE;
			float vertex_y = getHeightFromMap(j, i, image, width, height);
			float vertex_z = (float)i / ((float)VERTEX_COUNT - 1) * SIZE;
			//Setup the normals.
			float normal_x = 0;
			float normal_y = 1.0f;
			float normal_z = 0;
			//Setup the texcoords.
			float texCoord_x = (float)j / ((float)VERTEX_COUNT - 1);
			float texCoord_y = (float)i / ((float)VERTEX_COUNT - 1);
			//Push back to vectors.
			vertices.push_back(glm::vec3(vertex_x, vertex_y, vertex_z));
			normals.push_back(glm::vec3(normal_x, normal_y, normal_z));
			texCoords.push_back(glm::vec2(texCoord_x, texCoord_y));
		}
	}
	//Setup the indices to draw based on indice points.
	for (int gz = 0; gz < VERTEX_COUNT - 1; gz++)
	{
		for (int gx = 0; gx < VERTEX_COUNT - 1; gx++)
		{
			int topLeft = (gz*VERTEX_COUNT) + gx;
			int topRight = topLeft + 1;
			int bottomLeft = ((gz + 1)*VERTEX_COUNT) + gx;
			int bottomRight = bottomLeft + 1;
			//Push back to indices.
			indices.push_back(topLeft);
			indices.push_back(bottomLeft);
			indices.push_back(topRight);
			indices.push_back(topRight);
			indices.push_back(bottomLeft);
			indices.push_back(bottomRight);
		}
	}
	//Add into container structs for rendering.
	for (int i = 0; i < VERTEX_COUNT * VERTEX_COUNT; i++)
	{
		//Throw everything into a container to hold all values.
		Container container;
		container.vertex = vertices[i];
		container.normal = normals[i];
		container.texCoord = texCoords[i];
		containers.push_back(container);
	}
	//Perform smoothing.
	diamond_square(0, VERTEX_COUNT - 1, 0, VERTEX_COUNT - 1, (int)glm::pow(2, n_smooth), (float)n_range);
	updateNormals();
	updateMaxMinHeight();
}

/* Returns the RGB value of the position (height). */
float Terrain::getHeightFromMap(int x, int y, unsigned char * image, int width, int height)
{
	if (x < 0 || (3*x) >= (width) || y < 0 || y >= (height))
	{
		return 0;
	}
	int index = (3*x) + (width*y);
	float value = (float)image[index]+(float)image[index + 1]+(float)image[index + 2];
	float result = (float)fmod(value, (float)MAX_HEIGHT);
	return result;
}

/* Perform the diamond square algorithm at most 2^n steps. Pass in input level: 2^n, and small number for the range. */
void Terrain::diamond_square(int x1, int x2, int y1, int y2, int level, float range)
{
	//Check base case to stop recursion.
	if (level < 1)
		return;
	//Start random number generation.
	srand((unsigned int)time(NULL));
	//Diamond Algorithm
	for (int i = x1 + level; i < x2; i += level)
	{
		for (int j = y1 + level; j < y2; j += level)
		{
			//Get the 4 main vertices.
			glm::vec3 vec_a = this->vertices[(j - level)*VERTEX_COUNT + (i - level)];
			glm::vec3 vec_b = this->vertices[(j - level)*VERTEX_COUNT + i];
			glm::vec3 vec_c = this->vertices[(j)*VERTEX_COUNT + (i - level)];
			glm::vec3 vec_d = this->vertices[(j)*VERTEX_COUNT + i];
			//Get the middle vertex.
			glm::vec3 vec_e = this->vertices[(j - level / 2)*VERTEX_COUNT + (i - level / 2)];
			//Get the 4 heights.
			float height_a = vec_a.y;
			float height_b = vec_b.y;
			float height_c = vec_c.y;
			float height_d = vec_d.y;
			//Calculate the average height in the middle and set it to E.
			float height_e = (float)(height_a + height_b + height_c + height_d) / 4;
			height_e += fmod(((float)(rand()) / 1000), MAX_DISPLACEMENT)*range;
			vec_e.y = height_e;
			this->vertices[(j - level / 2)*VERTEX_COUNT + (i - level / 2)] = vec_e;
			this->containers[(j - level / 2)*VERTEX_COUNT + (i - level / 2)].vertex = vec_e;
		}
	}
	//Square algorithm
	for (int i = x1 + 2 * level; i < x2; i += level)
	{
		for (int j = y1 + 2 * level; j < y2; j += level)
		{
			//Get the 4 main vertices.
			glm::vec3 vec_a = this->vertices[(j - level)*VERTEX_COUNT + (i - level)];
			glm::vec3 vec_b = this->vertices[(j - level)*VERTEX_COUNT + i];
			glm::vec3 vec_c = this->vertices[(j)*VERTEX_COUNT + (i - level)];
			glm::vec3 vec_d = this->vertices[(j)*VERTEX_COUNT + i];
			//Get the middle vertex.
			glm::vec3 vec_e = this->vertices[(j - level / 2)*VERTEX_COUNT + (i - level / 2)];
			//Get the 5 heights.
			float height_a = vec_a.y;
			float height_b = vec_b.y;
			float height_c = vec_c.y;
			float height_d = vec_d.y;
			float height_e = vec_e.y;
			//Calculate the average height and set it to F.
			glm::vec3 vec_f = this->vertices[(j - level / 2)*VERTEX_COUNT + (i - level)];//
			float height_f = (float)(height_a + height_c + height_e + this->vertices[(j - level / 2)*VERTEX_COUNT + (i - 3 * level / 2)].y) / 3;
			height_f += fmod(((float)(rand()) / 1000), MAX_DISPLACEMENT)*range;
			vec_f.y = height_f;
			this->vertices[(j - level / 2)*VERTEX_COUNT + (i - level)] = vec_f;
			this->containers[(j - level / 2)*VERTEX_COUNT + (i - level)].vertex = vec_f;
			//Calculate the average height and set it to G.
			glm::vec3 vec_g = this->vertices[(j - level)*VERTEX_COUNT + (i - level / 2)];
			float height_g = (float)(height_a + height_b + height_e + this->vertices[(j - 3 * level / 2)*VERTEX_COUNT + (i - level / 2)].y) / 3;
			height_g += fmod(((float)(rand()) / 1000), MAX_DISPLACEMENT)*range;
			vec_g.y = height_g;
			this->vertices[(j - level)*VERTEX_COUNT + (i - level / 2)] = vec_g;
			this->containers[(j - level)*VERTEX_COUNT + (i - level / 2)].vertex = vec_g;
		}
	}
	//Begin Recursion.
	diamond_square(x1, x2, y1, y2, level / 2, range / 2);
}

/* Updates the normals for the entire terrain. */
void Terrain::updateNormals()
{
	for (int gz = 0; gz < VERTEX_COUNT - 1; gz++)
	{
		for (int gx = 0; gx < VERTEX_COUNT - 1; gx++)
		{
			int topLeft = (gz*VERTEX_COUNT) + gx;
			int topRight = topLeft + 1;
			int bottomLeft = ((gz + 1)*VERTEX_COUNT) + gx;
			int bottomRight = bottomLeft + 1;
			//Set the new normal values.
			glm::vec3 normal_TL = glm::cross(this->vertices[topLeft], this->vertices[bottomLeft]);
			this->normals[topLeft] = normal_TL;
			this->containers[topLeft].normal = normal_TL;
			glm::vec3 normal_BL = glm::cross(this->vertices[bottomLeft], this->vertices[topRight]);
			this->normals[bottomLeft] = normal_BL;
			this->containers[bottomLeft].normal = normal_BL;
			glm::vec3 normal_TR = glm::cross(this->vertices[topRight], this->vertices[bottomLeft]);
			this->normals[topRight] = normal_TR;
			this->containers[topRight].normal = normal_TR;
			glm::vec3 normal_BR = glm::cross(this->vertices[bottomLeft], this->vertices[bottomRight]);
			this->normals[bottomRight] = normal_BR;
			this->containers[bottomRight].normal = normal_BR;
		}
	}
}

/* Updates and finds the max and min height of the terrain. */
void Terrain::updateMaxMinHeight()
{
	float max = -INFINITY, min = INFINITY;
	for (int i = 0; i < vertices.size(); i++)
	{
		float cur_height = vertices[i].y;
		if (cur_height > max)
		{
			max = cur_height;
		}
		if (cur_height < min)
		{
			min = cur_height;
		}
	}
	//Set the max and min heights found.
	this->max_height = max;
	this->min_height = min;
}

/** Load a ppm file from disk.
@input filename The location of the PPM file.  If the file is not found, an error message
will be printed and this function will return 0
@input width This will be modified to contain the width of the loaded image, or 0 if file not found
@input height This will be modified to contain the height of the loaded image, or 0 if file not found
@return Returns the RGB pixel data as interleaved unsigned chars (R0 G0 B0 R1 G1 B1 R2 G2 B2 .... etc) or 0 if an error ocured
**/
unsigned char * Terrain::loadPPM(const char* filename, int& width, int& height)
{
	const int BUFSIZE = 128;
	FILE* fp;
	unsigned int read;
	unsigned char* rawData;
	char buf[3][BUFSIZE];
	char* retval_fgets;
	size_t retval_sscanf;
	//Read in the ppm file.
	if ((fp = fopen(filename, "rb")) == NULL)
	{
		std::cerr << "error reading ppm file, could not locate " << filename << std::endl;
		width = 0;
		height = 0;
		return NULL;
	}
	//Read magic number:
	retval_fgets = fgets(buf[0], BUFSIZE, fp);
	//Read width and height:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');
	retval_sscanf = sscanf(buf[0], "%s %s", buf[1], buf[2]);
	width = atoi(buf[1]);
	height = atoi(buf[2]);
	//Read maxval:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');
	//Read image data:
	rawData = new unsigned char[width * height * 3];
	read = (unsigned int)fread(rawData, width * height * 3, 1, fp);
	fclose(fp);
	if (read != 1)
	{
		std::cerr << "error parsing ppm file, incomplete data" << std::endl;
		delete[] rawData;
		width = 0;
		height = 0;
		return NULL;
	}
	return rawData;//Return rawData or 0 if failed.
}

/* Load the Terrain through the read height map. */
GLuint Terrain::loadTerrain(const char* filename, int index)
{
	//Hold the textureID (This will be the textureID to return).
	GLuint textureID;
	//Define variables to hold height map's width, height, pixel information.
	int width, height;
	unsigned char * image;
	//Create ID for texture.
	glGenTextures(1, &textureID);
	//Set the active texture ID.
	glActiveTexture(GL_TEXTURE0);
	//Set this texture to be the one we are working with.
	glBindTexture(GL_TEXTURE_2D, textureID);
	//Generate the texture.
	image = loadPPM(filename, width, height);//Load the ppm file.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	//Make sure no bytes are padded:
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//Use bilinear interpolation:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//Use clamp to edge:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);//X
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);//Y
	//Unbind the texture cube map.
	glBindTexture(GL_TEXTURE_2D, 0);
	//Return the textureID, we need to keep track of this texture variable.
	return textureID;
}

/* Initialize a terrain based on height maps. We can choose to generate a default height map or read in from an image ".ppm" file. */
void Terrain::setupTerrain(const char* terrain_0, const char* terrain_1, const char* terrain_2, const char* terrain_3, const char* blend_map)
{
	//Create buffers/arrays.
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	//Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO); //Bind vertex array object.

	glBindBuffer(GL_ARRAY_BUFFER, VBO); //Bind Container buffer.
	glBufferData(GL_ARRAY_BUFFER, this->containers.size() * sizeof(Container), &this->containers[0], GL_STATIC_DRAW); //Set vertex buffer to the Container.

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); //Bind indices buffer.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(int), &this->indices[0], GL_STATIC_DRAW);

	//Vertex Positions.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,//This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, //This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component).
		GL_FLOAT, //What type these components are.
		GL_FALSE, //GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't.
		sizeof(Container), //Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between.
		(GLvoid*)0); //Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	//Vertex Normals.
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Container), (GLvoid*)offsetof(Container, normal));

	//Vertex Texture Coords.
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Container), (GLvoid*)offsetof(Container, texCoord));

	//Set up Terrain textures.
	this->terrainTexture_0 = loadTerrain(terrain_0, 0);
	this->terrainTexture_1 = loadTerrain(terrain_1, 0);
	this->terrainTexture_2 = loadTerrain(terrain_2, 0);
	this->terrainTexture_3 = loadTerrain(terrain_3, 0);
	this->blendMap = loadTerrain(blend_map, 4);

	//Unbind.
	glBindBuffer(GL_ARRAY_BUFFER, 0); //Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind.
	glBindVertexArray(0); //Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO.
}

/* Toggle the draw mode to draw the mesh as lines (wireframe) or as triangle faces. */
void Terrain::toggleDrawMode()
{
	if (draw_mode == 0)
	{
		draw_mode = DRAW_WIREFRAME;
	}
	else
	{
		draw_mode = DRAW_SHADED;
	}
}

/* Draw the terrain. */
void Terrain::draw(GLuint shaderProgram)
{
	//Calculate combination of the model (toWorld), view (camera inverse), and perspective matrices. Send to shader.
	glm::mat4 MVP = Window::P * Window::V * this->toWorld;
	glm::mat4 model = this->toWorld;//We don't really need this, but we'll pass it through just in case.
	glm::mat4 view = glm::mat4(glm::mat3(Window::V));//Remove translation from the view matrix.
	glm::mat4 projection = Window::P;
	//Set MVP(Total calculated, easier to multiply in the shader) for the shader.
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	//Set individual components for shader calculations (Model, View, Projection).
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
	//Update heights.
	glUniform1f(glGetUniformLocation(shaderProgram, "max_height"), this->max_height);
	glUniform1f(glGetUniformLocation(shaderProgram, "min_height"), this->min_height);

	glDisable(GL_CULL_FACE);
	//Set draw_mode to view wireframe version or filled version.
	if (draw_mode == DRAW_SHADED)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else if (draw_mode == DRAW_WIREFRAME)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	//Draw the terrain.
	glBindVertexArray(VAO);//Bind the vertex.
	
	glActiveTexture(GL_TEXTURE0);//Enable the texture.
	glBindTexture(GL_TEXTURE_2D, this->terrainTexture_0);
	glUniform1i(glGetUniformLocation(shaderProgram, "TerrainTexture_0"), 0);

	glActiveTexture(GL_TEXTURE1);//Enable the texture.
	glBindTexture(GL_TEXTURE_2D, this->terrainTexture_1);
	glUniform1i(glGetUniformLocation(shaderProgram, "TerrainTexture_1"), 1);

	glActiveTexture(GL_TEXTURE2);//Enable the texture.
	glBindTexture(GL_TEXTURE_2D, this->terrainTexture_2);
	glUniform1i(glGetUniformLocation(shaderProgram, "TerrainTexture_2"), 2);

	glActiveTexture(GL_TEXTURE3);//Enable the texture.
	glBindTexture(GL_TEXTURE_2D, this->terrainTexture_3);
	glUniform1i(glGetUniformLocation(shaderProgram, "TerrainTexture_3"), 3);

	glActiveTexture(GL_TEXTURE4);//Enable the texture.
	glBindTexture(GL_TEXTURE_2D, this->blendMap);
	glUniform1i(glGetUniformLocation(shaderProgram, "blendMap"), 4);

	glDrawElements(GL_TRIANGLES, (GLsizei)this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);//Unbind vertex.

	//Set it back to fill.
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
