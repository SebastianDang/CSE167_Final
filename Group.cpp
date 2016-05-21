#include "Group.h"

using namespace std;

/* Define a Group Constructor amd initialize the Group. */
Group::Group() 
{
}

/* Deconstructor to safely delete when finished. */
Group::~Group()
{
}

/* Add a child (Node) to the list of nodes. */
void Group::addChild(Node * node) 
{
	children.push_back(node);
}

/* Remove a child (Node) to the list of nodes. */
void Group::removeChild(Node * node)
{
	children.remove(node);
}

/* Traverse the list of children and call each child node's draw function. */
void Group::draw(GLuint shaderProgram)
{
	for (list<Node*>::iterator it = children.begin(); it != children.end(); ++it) {
		(*it)->draw(shaderProgram);
	}

}

/* Traverse the list of children and call each child node's update function. */
void Group::update(glm::mat4 C)
{
	for (list<Node*>::iterator it = children.begin(); it != children.end(); ++it) {
		(*it)->update(C);
	}
}
