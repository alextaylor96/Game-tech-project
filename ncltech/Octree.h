#pragma once

#include "GameObject.h"
#include "Node.h"



class Octree
{
public:
	//creates the new octree 
	Octree(std::vector<GameObject*> &objects) {
		m_objects = objects;
		root = new Node();
		for (int i = 1; i < m_objects.size(); ++i) {
			root->m_objects.push_back(m_objects.at(i)->physicsNode);
		}

		root->createChildren();
	}

	~Octree();
	
	//sorts objects within the octree and updates them
	void update() {
		delete root;

		root = new Node();
		for (int i = 1; i < m_objects.size(); ++i) {
			root->m_objects.push_back(m_objects.at(i)->physicsNode);
		}

		root->createChildren();
	}

	//physics objects in this octree
	vector<GameObject*> m_objects;
	//the root node
	Node* root;
};

