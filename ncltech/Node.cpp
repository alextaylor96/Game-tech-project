#include "Node.h"

void Node::DebugDraw()
{
	for (int i = 0; i < m_childNodes.size(); ++i) {
		m_childNodes.at(i).DebugDraw();
	}
	NCLDebug::DrawThickLine(Vector3(m_region._max.x, m_region._max.y, m_region._max.z),
		Vector3(m_region._min.x, m_region._max.y, m_region._max.z),
		0.02f, Vector4(0.f, 0.f, 0.f, 1.f));

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

void Node::createChildren() {
	//the middle of each side in the current nodes
	float midX = (m_region._max.x + m_region._min.x) / 2;
	float midY = (m_region._max.y + m_region._min.y) / 2;
	float midZ = (m_region._max.z + m_region._min.z) / 2;

	Node child1(this,
		Vector3(midX, midY, midZ),
		Vector3(m_region._min.x, m_region._min.y, m_region._min.z)
	);

	Node child2(this,
		Vector3(midX, m_region._max.y, midZ),
		Vector3(m_region._min.x, midY, m_region._min.z)
	);

	Node child3(this,
		Vector3(m_region._max.x, midY, midZ),
		Vector3(midX, m_region._min.y, m_region._min.z)
	);

	Node child4(this,
		Vector3(m_region._max.x, m_region._max.y , midZ),
		Vector3(midX, midY , m_region._min.z)
	);

	Node child5(this,
		Vector3(midX, midY, m_region._max.z),
		Vector3(m_region._min.x, m_region._min.y, midZ)
	);

	Node child6(this,
		Vector3(midX, m_region._max.y, m_region._max.z),
		Vector3(m_region._min.x, midY, midZ)
	);


	Node child7(this,
		Vector3(m_region._max.x , midY, m_region._max.z),
		Vector3(midX, m_region._min.y, midZ)
	);


	Node child8(this,
		Vector3(m_region._max.x, m_region._max.y, m_region._max.z),
		Vector3(midX, midY, midZ)
	);

	m_childNodes.push_back(child1);
	m_childNodes.push_back(child2);
	m_childNodes.push_back(child3);
	m_childNodes.push_back(child4);
	m_childNodes.push_back(child5);
	m_childNodes.push_back(child6);
	m_childNodes.push_back(child7);
	m_childNodes.push_back(child8);

	sortObjects();
}

void Node::sortObjects()
{
	for (int i = 0; i < m_objects.size(); ++i) {
		Vector3 objPos = m_objects.at(i)->GetPosition();
		float objRad = m_objects.at(i)->GetCollisionShape()->GetRadius();
		//check if current object is in any of the child nodes
		for (int j = 0; j < m_childNodes.size(); ++j) {
			float maxX = m_childNodes.at(j).m_region._max.x + objRad;
			float maxY = m_childNodes.at(j).m_region._max.y + objRad;
			float maxZ = m_childNodes.at(j).m_region._max.z + objRad;

			float minX = m_childNodes.at(j).m_region._min.x - objRad;
			float minY = m_childNodes.at(j).m_region._min.y - objRad;
			float minZ = m_childNodes.at(j).m_region._min.z - objRad;
			
			if (objPos.x <= maxX && objPos.x >= minX) {
				if (objPos.y <= maxY && objPos.y >= minY) {
					if (objPos.z <= maxZ && objPos.z >= minZ) {
						//inside the node
						m_childNodes.at(j).m_objects.push_back(m_objects.at(i));
					}
				}
			}

		}
	}
	//all objects sorted into children
	for (int k = 0; k < m_childNodes.size(); ++k) {
		if (m_childNodes.at(k).m_objects.size() >= THRESHOLD) {
			m_childNodes.at(k).createChildren();
		}
	}
}
