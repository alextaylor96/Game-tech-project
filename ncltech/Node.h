#pragma once

#include "BoundingBox.h"
#include "Scene.h"

#define THRESHOLD 5

class Node{
public:
	//gets called only when creating the first node
	Node() {
		m_region._max = Vector3(20.f, 20.f, 20.f);
		m_region._min = Vector3(-20.f, 0.f, -20.f);
	}

	Node(Node* parent, Vector3 max, Vector3 min) {
		m_parent = parent;
		m_region._max = max;
		m_region._min = min;
	}

	~Node() {};

	//draws the nodes bounding box
	void DebugDraw();

	//create 8 child nodes
	void createChildren();

	//sort the objects in this node into its children
	void sortObjects();

	//the region of the box
	BoundingBox m_region;

	//physics objects in this box
	vector<PhysicsNode*> m_objects;

	//the possible child nodes of this node(up to 8)
	vector<Node> m_childNodes;

	//the smallest the bounding box can be
	const int minSize = 1;


	//pointer to parent node
	Node* m_parent = nullptr;
};