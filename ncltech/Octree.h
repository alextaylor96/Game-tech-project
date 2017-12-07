#pragma once

#include "GameObject.h"
#include "Node.h"

#define THRESHOLD 10

class Octree
{
public:
	//creates the new octree 
	Octree(std::vector<GameObject*> &objects) {
		m_objects = objects;
		root = new Node();
		root->createChildren();
	}

	~Octree();
	
	//sorts objects within the octree and updates them
	void sortObjects() {
	}

	//physics objects in this octree
	vector<GameObject*> m_objects;
	//the root node
	Node* root;
};

