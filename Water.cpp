//
//  Water.cpp
//  Project1
//
//  Created by Parth Patel on 5/27/16.
//  Copyright © 2016 Parth Patel. All rights reserved.
//

#include <stdio.h>
#include "Water.h"
#include "Window.h"

#define SIZE 125
#define HEIGHT 0

//4x4 grid of points that will define the surface
Point Points[4][4] = {
    {
        { 2*SIZE, HEIGHT, (2 * SIZE) },
        { SIZE, HEIGHT, (2*SIZE) },
        { -SIZE ,HEIGHT, (2 * SIZE) },
        {-(2 * SIZE), HEIGHT, (2 * SIZE) }
    },
    {
        { (2 * SIZE), HEIGHT, SIZE },
        { (2 * SIZE), HEIGHT, SIZE },
        { -SIZE, HEIGHT, SIZE },
        {-(2 * SIZE), HEIGHT, SIZE }
    },
    {
        { (2 * SIZE), HEIGHT, -SIZE },
        {  SIZE , HEIGHT, -SIZE },
        { -SIZE, HEIGHT,-SIZE },
        {-(2 * SIZE), HEIGHT, -SIZE }
    },
    {
        { (2*SIZE),HEIGHT, -(2*SIZE) },
        {  SIZE,HEIGHT , -(2*SIZE) },
        { -SIZE,HEIGHT , -(2*SIZE) },
        {-(2*SIZE),HEIGHT,-(2*SIZE) }
    }
};

unsigned int LOD = 20;
std::vector<glm::vec3> verticiesList;
std::vector<glm::vec3> normalsList;
std::vector<int> indiciesList;

Water::Water()
{
    this->wireframe = 0;
    
    this->toWorld = glm::mat4(1.0f);
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(10, 0.0, 10));
	this->toWorld = translate * toWorld;
    
    /* Setting up verticies */
    // use the parametric time value 0 to 1
    for(int i=0;i!=LOD;++i) {
        
        // calculate the parametric u value
        float u = (float)i/(LOD-1);
        
        for(int j=0;j!=LOD;++j) {

            
            // calculate the parametric v value
            float v = (float)j/(LOD-1);
            
            // calculate the point on the surface
            Point p = Calculate(u,v);
            
            // draw point
            verticiesList.push_back(glm::vec3(p.x,p.y,p.z));
            normalsList.push_back((glm::vec3(0.0, 1.0, 0.0)));
            //glVertex3f(p.x,p.y,p.z);
        }
    }
    
    /* Setting up indicies */
    //Setup the indices to draw based on indice points.
    for (int gx = 0; gx < LOD-1; ++gx)
    {
        for (int gz = 0; gz < LOD-1; ++gz)
        {
           // std::cout << "GX: " << gx << std::endl;
           // std::cout << "GZ: " << gz << std::endl;
            int topLeft = (gx*LOD) + gz;
            int topRight = ((gx+1)*LOD) + gz;
            int bottomLeft = topLeft + 1;
            int bottomRight = topRight + 1;
            //Push back to indices.
            indiciesList.push_back(topLeft);
            indiciesList.push_back(bottomLeft);
            indiciesList.push_back(topRight);
            indiciesList.push_back(topRight);
            indiciesList.push_back(bottomLeft);
            indiciesList.push_back(bottomRight);
        }
    }
    
    /* Setting up normals */
    /* Updates the normals for the entire terrain. */
        for (int i = 0; i!=LOD;++i)
        {
            for (int j = 0; j!=LOD;++j)
            {
				/*
				int topLeft = (j*LOD) + i;
                int topRight = ((j+1)*LOD) + i;
                int bottomLeft = topLeft + 1;
                int bottomRight = topRight + 1;
                glm::vec3 normal_TL = glm::cross(verticiesList[topLeft], verticiesList[bottomLeft]);
                normalsList[topLeft] = normal_TL;
                glm::vec3 normal_BL = glm::cross(verticiesList[bottomLeft], verticiesList[topRight]);
                normalsList[bottomLeft] = normal_BL;
                glm::vec3 normal_TR = glm::cross(verticiesList[topRight], verticiesList[bottomLeft]);
                normalsList[topRight] = normal_TR;
                glm::vec3 normal_BR = glm::cross(verticiesList[bottomLeft], verticiesList[bottomRight]);
                normalsList[bottomRight] = normal_BR;
				
				*/
				
				float heightL = getHeightFromVertex(j - 1, i);
				float heightR = getHeightFromVertex(j + 1, i);
				float heightD = getHeightFromVertex(j, i + 1);
				float heightU = getHeightFromVertex(j, i - 1);
				glm::vec3 normal = glm::normalize(glm::vec3(heightL - heightR, 2.0f, heightU - heightD));
				normalsList[(i*LOD) + j] = normal;
				
            }
        }


    
    
    // Create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &VBONORM);
    
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);
    
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verticiesList.size() * sizeof(glm::vec3), &verticiesList[0], GL_DYNAMIC_DRAW);
    

    
    glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
                          3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
                          GL_FLOAT, // What type these components are
                          GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
                          3 * sizeof(GLfloat), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
                          (GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
    glEnableVertexAttribArray(0);

    
    
    glBindBuffer(GL_ARRAY_BUFFER, VBONORM);
    glBufferData(GL_ARRAY_BUFFER, normalsList.size() * sizeof(glm::vec3), &normalsList[0], GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indiciesList.size() * sizeof(unsigned int), &indiciesList[0], GL_DYNAMIC_DRAW);
    

    
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
    
    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
    
    std::cout << "Verticies Count: " << verticiesList.size() << std::endl;
    std::cout << "Indicies Count: " << indiciesList.size() << std::endl;
    std::cout << "Normals Count: " << normalsList.size() << std::endl;
}


Water::~Water()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &VBONORM);
    glDeleteBuffers(1, &EBO);
}


Point Water::CalculateU(float t,int row) {
    
    // the final point
    Point p;
    
    // the t value inverted
    float it = 1.0f-t;
    
    // calculate blending functions
    float b0 = t*t*t;
    float b1 = 3*t*t*it;
    float b2 = 3*t*it*it;
    float b3 =  it*it*it;
    
    // sum the effects of the Points and their respective blending functions
    p.x = b0*Points[row][0].x +
		  b1*Points[row][1].x +
		  b2*Points[row][2].x +
		  b3*Points[row][3].x ;
    
    p.y = b0*Points[row][0].y +
		  b1*Points[row][1].y +
		  b2*Points[row][2].y +
		  b3*Points[row][3].y ;
    
    p.z = b0*Points[row][0].z +
		  b1*Points[row][1].z +
		  b2*Points[row][2].z +
		  b3*Points[row][3].z ;
    
    return p;
}

Point Water::CalculateV(float t,Point* pnts) {
    Point p;
    
    // the t value inverted
    float it = 1.0f-t;
    
    // calculate blending functions
    float b0 = t*t*t;
    float b1 = 3*t*t*it;
    float b2 = 3*t*it*it;
    float b3 =  it*it*it;
    
    // sum the effects of the Points and their respective blending functions
    p.x = b0*pnts[0].x +
		  b1*pnts[1].x +
		  b2*pnts[2].x +
		  b3*pnts[3].x ;
    
    p.y = b0*pnts[0].y +
		  b1*pnts[1].y +
		  b2*pnts[2].y +
		  b3*pnts[3].y ;
    
    p.z = b0*pnts[0].z +
		  b1*pnts[1].z +
		  b2*pnts[2].z + 
		  b3*pnts[3].z ;
    
    
    
    
    return p;
}

Point Water::Calculate(float u,float v) {
    
    // first of all we will need to evaluate 4 curves in the u
    // direction. The points from those will be stored in this
    // temporary array
    Point temp[4];
    
    // calculate each point on our final v curve
    temp[0] = CalculateU(u,0);
    temp[1] = CalculateU(u,1);
    temp[2] = CalculateU(u,2);
    temp[3] = CalculateU(u,3);
    
    // having got 4 points, we can use it as a bezier curve
    // to calculate the v direction. This should give us our
    // final point
    //
    return CalculateV(v,temp);
}





void Water::draw(GLuint shaderProgram)
{

    //Start wireframe mode
    if(wireframe == 1) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    float time = (float)glfwGetTime();
    
    
    //toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(75, 0, -75));
    //toWorld = glm::scale(glm::mat4(1.0f), glm::vec3(10, 1, 10)) * toWorld;

    
    // Calculate combination of the model (toWorld), view (camera inverse), and perspective matrices
    glm::mat4 MVP = Window::P * Window::V * toWorld;
    // We need to calculate this because as of GLSL version 1.40 (OpenGL 3.1, released March 2009), gl_ModelViewProjectionMatrix has been
    // removed from the language. The user is expected to supply this matrix to the shader when using modern OpenGL.
    GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    
    MatrixID = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &toWorld[0][0]);
    
    glm::vec3 currCamera = Window::camera_pos;
    glUniform3f(glGetUniformLocation(shaderProgram, "viewCam"), currCamera.x, currCamera.y, currCamera.z);
    GLint loc = glGetUniformLocation(shaderProgram, "randVal");

    
    int myRandom = rand() % 10001;
    double number = myRandom * 0.000001;
    
    loc = glGetUniformLocation(shaderProgram, "time");
    glUniform1f(loc, time);

    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, indiciesList.size(), GL_UNSIGNED_INT, 0);
    
    glBindVertexArray(0);
    
    
    //End wireframe mode
    if(wireframe == 1) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

/* Return the height at a given x, y coordinate. */
float Water::getHeightFromVertex(int x, int y)
{
	if (x < 0 || (x) >= LOD || y < 0 || y >= LOD)
	{
		return 0;
	}
	return normalsList[(y*LOD) + x].y;
}