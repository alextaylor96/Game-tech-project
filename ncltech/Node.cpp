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

void Node::createChildren(){
	//the middle of each side in the current nodes
	float midX = (m_region._max.x + m_region._min.x) / 2;
	float midY = (m_region._max.y + m_region._min.y) / 2;
	float midZ = (m_region._max.z + m_region._min.z) / 2;

	Node child1(this,
		Vector3(m_region._max.x, m_region._max.y, m_region._max.z),
		Vector3(midX, midY, midZ)
	);

	m_childNodes.push_back(child1);
}
