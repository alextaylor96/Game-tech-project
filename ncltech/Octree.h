#pragma once

#include "GameObject.h"
#include "Node.h"



class Octree
{
public:
	//creates the new octree 
	Octree(std::vector<PhysicsNode*> *objects) {
		m_objects = objects;
		root = new Node();
		root->m_objects = *m_objects;
		root->createChildren();
	}

	~Octree();
	
	//sorts objects within the octree and updates them
	void update() {
		delete root;

		root = new Node();
		root->m_objects = *m_objects;

		root->createChildren();
	}

	void addObject(PhysicsNode* obj) {
		m_objects->push_back(obj);
	}


	//physics objects in this octree
	vector<PhysicsNode*>* m_objects;
	//the root node
	Node* root;
};

