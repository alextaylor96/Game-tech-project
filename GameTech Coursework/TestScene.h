#pragma once

#include <ncltech\Scene.h>

class TestScene : public Scene
{
public:
	TestScene(const std::string& friendly_name);
	virtual ~TestScene();

	virtual void OnInitializeScene()	 override;
	virtual void OnCleanupScene()		 override;
	virtual void OnUpdateScene(float dt) override;

	bool OnCollisionCallback(PhysicsNode* self, PhysicsNode* otherObject);

protected:
	float m_AccumTime;
	GameObject* m_pPlayer;
};