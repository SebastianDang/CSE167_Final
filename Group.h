#pragma once
#ifndef GROUP_H
#define GROUP_H

#include "Window.h"
#include "Node.h"

/* Group should store a list of pointers to child nodes (std::list<Node*>) and provide functionality to add and remove child nodes 
   (addChild(), removeChild()). Its draw method needs to traverse the list of children and call each child node's draw function. */
class Group : public Node 
{
private:
public:
	//Constructor methods.
	Group();
	~Group();
	
	//Handle list of children.
	std::list<Node*> children;
	void addChild(Node* node);
	void removeChild(Node* node);

	//Draw and upate methods.
	void draw(GLuint shaderProgram);
	void update(glm::mat4 C);
};
#endif