#include "Terrain.h"

using namespace std;

#define SIZE 100//800
#define VERTEX_COUNT 128//128

/* Procedurally generated Terrain. Ability to input a height map: either real or generated from different applications. Shader that adds at least 3 different type of terrain(grass, desert, snow). */
Terrain::Terrain()
{
	this->x = SIZE;
	this->z = SIZE;

	this->toWorld = glm::mat4(1.0f);
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3((float)SIZE / 2.0f, 0, (float)SIZE / 2.0f));
	this->toWorld = translate*this->toWorld;

	this->loadTerrain("TEST");
	this->setupTerrain();
}

/* Deconstructor to safely delete when finished. */
Terrain::~Terrain()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
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
void Terrain::loadTerrain(const char* filename)
{
	//Define variables to hold height map's width, height, pixel information.
	//int width, height;
	//unsigned char * image;

	//image = loadPPM(filename, width, height);//Load the ppm file.

	//Create the height map: v, vn, texCoords

	/* -------------------- TODO: WORK IN PROGRESS -------------------- */

	//Generate vertices, normals, and texCoords for a terrain map.
	for (int i = 0; i < VERTEX_COUNT; i++)
	{
		for (int j = 0; j < VERTEX_COUNT; j++)
		{
			//Setup the vertices.
			float vertex_x = -(float)j / ((float)VERTEX_COUNT - 1) * SIZE;
			float vertex_y = 0.0f;//0
			float vertex_z = -(float)i / ((float)VERTEX_COUNT - 1) * SIZE;
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
		container.texCoord = glm::vec2(0.0f, 0.0f);
		containers.push_back(container);
	}
}

/* Initialize a terrain based on height maps. We can choose to generate a default height map or read in from an image ".ppm" file. */
void Terrain::setupTerrain()
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

	//Unbind.
	glBindBuffer(GL_ARRAY_BUFFER, 0); //Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind.
	glBindVertexArray(0); //Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO.
}

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
	//Update the viewPosition and intensity.
	//glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), Window::camera_pos.x, Window::camera_pos.y, Window::camera_pos.z);
	//glUniform1f(glGetUniformLocation(shaderProgram, "reflect_intensity"), 50.0f / 100.0f);
	//Draw Terrain
	glBindVertexArray(VAO);//Bind the vertex.
	glDrawElements(GL_TRIANGLES, (GLsizei)this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);//Unbind vertex.
}