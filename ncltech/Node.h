#pragma once

#include "BoundingBox.h"
#include "Scene.h"

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

	void createChildren();

	//the region of the box
	BoundingBox m_region;

	//physics objects in this box
	vector<GameObject*> m_objects;

	//the possible child nodes of this node(up to 8)
	vector<Node> m_childNodes;

	//the smallest the bounding box can be
	const int minSize = 1;


	//pointer to parent node
	Node* m_parent = nullptr;
};