#pragma once

#include <ncltech\Scene.h>
#include <ncltech\CommonUtils.h>
#include <ncltech\DistanceConstraint.h>

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

		GameObject* ball;

		ball = CommonUtils::BuildSphereObject(
			"sphere",
			Vector3(5.0f, 5.0f, 0.0f), 0.5f , true, 0.5f, true, true, Vector4(1, 0, 0, 1)
		);

		this->AddGameObject(ball);

		GameObject* attachedTo;

		attachedTo = CommonUtils::BuildSphereObject("",
			Vector3(0.f, 10.f, 0.0f),				//Position
			0.1f,									//Radius
			true,									//Has Physics Object
			0.1f,									//Inverse Mass = 1 / 10 kg mass (For creating rotational inertia tensor)
			false,									//No Collision Shape Yet
			true,									//Dragable by the user
			Vector4(0,0,0,1));	//Color

													//Set linear mass to be infinite, so it can rotate still but not move
		attachedTo->Physics()->SetInverseMass(0.0f);

		this->AddGameObject(attachedTo);
		//attach the ball to the contraint thing
		DistanceConstraint* constraint = new DistanceConstraint(
			ball->Physics(),					//Physics Object A
			attachedTo->Physics(),					//Physics Object B
			ball->Physics()->GetPosition(),	//Attachment Position on Object A	-> Currently the centre
			attachedTo->Physics()->GetPosition());	//Attachment Position on Object B	-> Currently the centre  

	
		PhysicsEngine::Instance()->AddConstraint(constraint);

	}

};