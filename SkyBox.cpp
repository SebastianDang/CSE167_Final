#include "SkyBox.h"
#include "Window.h"

#define SIZE 500.0f

/* Initialize the object, load all the textures and set up buffers. */
SkyBox::SkyBox()
{
	//Set the skybox position in the world.
	this->toWorld = glm::mat4(1.0f);
	//Setup the cube for rendering (vertices).
	setupCube();
	//Setup the textures for each face, including loading the ppm files.
	setupFaces();
	//Setup the Sky box for rendering.
	setupSkyBox();
}

/* Deconstructor to safely delete when finished. */
SkyBox::~SkyBox()
{
	//Properly de-allocate all resources once they've outlived their purpose.
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

/* Setup the drawing/outline of the cube for the Sky Box. */
void SkyBox::setupCube()
{
	GLfloat vertices_array[] = {
		// Front vertices
		-SIZE, -SIZE,  SIZE,
		SIZE, -SIZE,  SIZE,
		SIZE,  SIZE,  SIZE,
		-SIZE,  SIZE,  SIZE,
		// Back vertices
		-SIZE, -SIZE, -SIZE,
		SIZE, -SIZE, -SIZE,
		SIZE,  SIZE, -SIZE,
		-SIZE,  SIZE, -SIZE
	};
	GLuint indices_array[] = {  // Note that we start from 0!
		//Front face
		0, 1, 2,
		2, 3, 0,
		//Top face
		1, 5, 6,
		6, 2, 1,
		//Back face
		7, 6, 5,
		5, 4, 7,
		//Bottom face
		4, 0, 3,
		3, 7, 4,
		//Left face
		4, 5, 1,
		1, 0, 4,
		//Right face
		3, 2, 6,
		6, 7, 3
	};
	//Front vertices.
	this->vertices.push_back(glm::vec3(-SIZE, -SIZE, SIZE));
	this->vertices.push_back(glm::vec3(SIZE, -SIZE, SIZE));
	this->vertices.push_back(glm::vec3(SIZE, SIZE, SIZE));
	this->vertices.push_back(glm::vec3(-SIZE, SIZE, SIZE));
	//Back vertices.
	this->vertices.push_back(glm::vec3(-SIZE, -SIZE, -SIZE));
	this->vertices.push_back(glm::vec3(SIZE, -SIZE, -SIZE));
	this->vertices.push_back(glm::vec3(SIZE, SIZE, -SIZE));
	this->vertices.push_back(glm::vec3(-SIZE, SIZE, -SIZE));
	//Faces.
	for (int i = 35; i >= 0; i--)
	{
		this->indices.push_back(indices_array[i]);
	}
}

/* Setup the Skybox faces. */
void SkyBox::setupFaces()
{
	//------------------------------ Windows (both 32 and 64 bit versions) ------------------------------ //
	#ifdef _WIN32 
	this->faces.push_back("../skybox/right.ppm");//Right.
	this->faces.push_back("../skybox/left.ppm");//Left.
	this->faces.push_back("../skybox/top.ppm");//Top.
	this->faces.push_back("../skybox/bottom.ppm");//Bottom.
	this->faces.push_back("../skybox/back.ppm");//Back.
	this->faces.push_back("../skybox/front.ppm");//Front.

	//----------------------------------- Not Windows (MAC OSX) ---------------------------------------- //
	#else
	this->faces.push_back("./skybox/right.ppm");//Right.
	this->faces.push_back("./skybox/left.ppm");//Left.
	this->faces.push_back("./skybox/top.ppm");//Top.
	this->faces.push_back("./skybox/bottom.ppm");//Bottom.
	this->faces.push_back("./skybox/back.ppm");//Back.
	this->faces.push_back("./skybox/front.ppm");//Front.
	#endif
}

/** Load a ppm file from disk.
@input filename The location of the PPM file.  If the file is not found, an error message
will be printed and this function will return 0
@input width This will be modified to contain the width of the loaded image, or 0 if file not found
@input height This will be modified to contain the height of the loaded image, or 0 if file not found
@return Returns the RGB pixel data as interleaved unsigned chars (R0 G0 B0 R1 G1 B1 R2 G2 B2 .... etc) or 0 if an error ocured
**/
unsigned char* SkyBox::loadPPM(const char* filename, int& width, int& height)
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

/* Load the cube map and return a texture ID. */
GLuint SkyBox::loadCubemap(std::vector<const GLchar*> faces)
{
	//Hold the textureID (This will be the textureID to return).
	GLuint textureID;

	//Define variables to hold height map's width, height, pixel information.
	int width, height;
	unsigned char * image;

	//Create ID for texture.
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);//Set this texture to be the active texture (0).
	//Set this texture to be the one we are working with.
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	//Generate the texture.
	for (GLuint i = 0; i < faces.size(); i++)
	{
		image = loadPPM(faces[i], width, height);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	}
	//Make sure no bytes are padded:
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//Select GL_MODULATE to mix texture with polygon color for shading:
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//Use bilinear interpolation:
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//Use clamp to edge to hide skybox edges:
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//X
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);//Y
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);//Z
	//Unbind the texture cube map.
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	//Return the textureID, we need to keep track of this texture variable.
	return textureID;
}

/* Setup the skybox for modern openGL rendering. */
void SkyBox::setupSkyBox()
{
	//Create buffers/arrays.
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	//Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO); //Bind vertex array object.

	glBindBuffer(GL_ARRAY_BUFFER, VBO); //Bind vertex buffer.
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * 3 * sizeof(GLfloat), &this->vertices[0], GL_STATIC_DRAW);//Set vertex buffer to vertices.

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); //Bind the EBO for the indices (faces). 
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(int), &this->indices[0], GL_STATIC_DRAW);

	//Vertex Positions.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	//Set up Skybox faces. We set this to cubemapTexture so we can call this later in draw.
	this->cubemapTexture = loadCubemap(faces);

	glBindBuffer(GL_ARRAY_BUFFER, 0); //Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind.

	glBindVertexArray(0); //Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO.
}

/* Draw the skybox based on the texture map. */
void SkyBox::draw(GLuint shaderProgram)
{
	//Calculate combination of the model (toWorld), view (camera inverse), and perspective matrices. Send to shader.
	glm::mat4 MVP = Window::P * Window::V * toWorld;
	glm::mat4 model = this->toWorld;//We don't really need this, but we'll pass it through just in case.
	glm::mat4 view = glm::mat4(glm::mat3(Window::V));//Remove translation from the view matrix.
	glm::mat4 projection = Window::P;
	//Set MVP(Total calculated, easier to multiply in the shader) for the shader.
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	//Set individual components for shader calculations (Model, View, Projection).
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
	//Draw skybox.
	glDepthMask(GL_FALSE);//Remember to turn depth writing off.
	//Skybox cube.
	glBindVertexArray(VAO);//Bind the vertex.
	glActiveTexture(GL_TEXTURE0);//Enable the texture.
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);//Bind the cubemapTexture.
	glDrawElements(GL_TRIANGLES, (GLsizei)this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);//Unbind vertex.
	glDepthMask(GL_TRUE);//Turn depth writing back on.
	//Culling.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

GLuint SkyBox::getSkyBox()
{
	return this->cubemapTexture;
}
