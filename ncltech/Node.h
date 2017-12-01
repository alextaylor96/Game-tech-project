#pragma once

#include "BoundingBox.h"
#include "Scene.h"

class Node{
public:
	//gets called only when creating the first node
	Node() {
		m_region._max = Vector3(20.f, 20.f, 20.f);
		m_region._min = Vector3(-20.f, -20.f, 0.f);
	}

	Node(Node* parent, Vector3 max, Vector3 min) {
		m_parent = parent;
	}

	~Node();

	//draws the nodes bounding box
	void DebugDraw() {
			//NCLDebug::DrawThickLine(Vector3(0.f, 0.f, 0.f), Vector3(10.f, 10.f, 0.f), 0.01f, Vector4(0.f, 0.f, 0.f, 1.f));

			NCLDebug::DrawThickLine(Vector3(m_region._max.x, m_region._max.y, m_region._max.z), 
				Vector3(m_region._min.x, m_region._max.y, m_region._max.z),
				0.02f,Vector4(0.f, 0.f, 0.f, 1.f));

			NCLDebug::DrawThickLine(Vector3(m_region._max.x, m_region._max.y, m_region._max.z),
				Vector3(m_region._max.x, m_region._min.y, m_region._max.z),
				0.02f, Vector4(0.f, 0.f, 0.f, 1.f));

			NCLDebug::DrawThickLine(Vector3(m_region._max.x, m_region._max.y, m_region._max.z),
				Vector3(m_region._max.x, m_region._max.y, m_region._min.z),
				0.02f, Vector4(0.f, 0.f, 0.f, 1.f));

			NCLDebug::DrawThickLine(Vector3(m_region._min.x, m_region._min.y, m_region._min.z),
				Vector3(m_region._max.x, m_region._min.y, m_region._min.z),
				0.02f, Vector4(0.f, 0.f, 0.f, 1.f));
	
			NCLDebug::DrawThickLine(Vector3(m_region._min.x, m_region._min.y, m_region._min.z),
				Vector3(m_region._min.x, m_region._min.y, m_region._max.z),
				0.02f, Vector4(0.f, 0.f, 0.f, 1.f));

			NCLDebug::DrawThickLine(Vector3(m_region._min.x, m_region._min.y, m_region._min.z),
				Vector3(m_region._min.x, m_region._max.y, m_region._min.z),
				0.02f, Vector4(0.f, 0.f, 0.f, 1.f));

			NCLDebug::DrawThickLine(Vector3(m_region._min.x, m_region._max.y, m_region._min.z),
				Vector3(m_region._max.x, m_region._max.y, m_region._min.z),
				0.02f, Vector4(0.f, 0.f, 0.f, 1.f));

			NCLDebug::DrawThickLine(Vector3(m_region._min.x, m_region._max.y, m_region._min.z),
				Vector3(m_region._min.x, m_region._max.y, m_region._max.z),
				0.02f, Vector4(0.f, 0.f, 0.f, 1.f));

			NCLDebug::DrawThickLine(Vector3(m_region._max.x, m_region._max.y, m_region._min.z),
				Vector3(m_region._max.x, m_region._min.y, m_region._min.z),
				0.02f, Vector4(0.f, 0.f, 0.f, 1.f));

			NCLDebug::DrawThickLine(Vector3(m_region._min.x, m_region._min.y, m_region._max.z),
				Vector3(m_region._min.x, m_region._max.y, m_region._max.z),
				0.02f, Vector4(0.f, 0.f, 0.f, 1.f));

			NCLDebug::DrawThickLine(Vector3(m_region._min.x, m_region._min.y, m_region._max.z),
				Vector3(m_region._max.x, m_region._min.y, m_region._max.z),
				0.02f, Vector4(0.f, 0.f, 0.f, 1.f));

			NCLDebug::DrawThickLine(Vector3(m_region._max.x, m_region._min.y, m_region._min.z),
				Vector3(m_region._max.x, m_region._min.y, m_region._max.z),
				0.02f, Vector4(0.f, 0.f, 0.f, 1.f));
	}

	//the region of the box
	BoundingBox m_region;

	//physics objects in this box
	vector<GameObject*> m_objects;

	//the possible child nodes of this node(up to 8)
	vector<Node*> m_childNodes;

	//the smallest the bounding box can be
	const int minSize = 1;


	//pointer to parent node
	Node* m_parent = nullptr;
};