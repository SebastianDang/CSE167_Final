//
//  Water.h
//  Project1
//
//  Created by Parth Patel on 5/27/16.
//  Copyright © 2016 Parth Patel. All rights reserved.
//

#ifndef Water_h
#define Water_h

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

/// a structure to hold a control point of the surface
struct Point {
    float x;
    float y;
    float z;
};


class Water
{
public:
    Water();
    ~Water();
    
    glm::mat4 toWorld;
    GLuint VBO, VAO, EBO, VBONORM;
    
    void draw(GLuint);
    int wireframe;
    GLuint setupDuDvMap();
        
    Point Calculate(float u,float v);
        
    Point CalculateV(float t,Point* pnts);
            
    Point CalculateU(float t,int row);

	float getHeightFromVertex(int x, int y);

};


#endif /* Water_h */
