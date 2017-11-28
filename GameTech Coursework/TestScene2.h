#pragma once

#include <ncltech\Scene.h>
#include <ncltech\CommonUtils.h>

//Fully striped back scene to use as a template for new scenes.
class TestScene2 : public Scene
{
public:
	TestScene2(const std::string& friendly_name)
		: Scene(friendly_name)
	{
	}

	virtual ~TestScene2()
	{
	}

	virtual void OnInitializeScene() override
	{
		Scene::OnInitializeScene();

		//Who doesn't love finding some common ground?
		this->AddGameObject(CommonUtils::BuildCuboidObject(
			"Ground",
			Vector3(0.0f, -1.5f, 0.0f),
			Vector3(20.0f, 1.0f, 20.0f),
			false,
			0.0f,
			false,
			false,
			Vector4(0.2f, 0.5f, 1.0f, 1.0f)));

		this->AddGameObject(CommonUtils::BuildSphereObject(
			"sphere",
			Vector3(0.0f, 1.0f, 0.0f), 0.5f , true, 0.5f, true, true, Vector4(1, 0, 0, 1)
		));
	}

};