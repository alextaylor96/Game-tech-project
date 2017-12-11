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
			Vector3(15.0f, 2.0f, 0.0f), 0.5f , true, 0.5f, true, true, Vector4(1, 0, 0, 1)
		);

		this->AddGameObject(ball);

		GameObject* attachedTo;

		attachedTo = CommonUtils::BuildSphereObject("",
			Vector3(10.f, 7.f, 0.0f),				//Position
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

		//create a soft body
		vector<GameObject*> clothNodes;
		int clothDimensions = 10;

		for (int i = 0; i < clothDimensions; ++i) {
			for (int j = 0; j < clothDimensions; ++j) {
				GameObject* cloth = CommonUtils::BuildSphereObject(
					"",
					Vector3(-5.0f + (j), 15.0f - (i), 0.0f), 0.1f, true, 0.5f, false, true, Vector4(0, 0, 1, 1)
				);
				//make top row cant move
				if (i == 0) {
					cloth->Physics()->SetInverseMass(0.0f);
				}
				this->AddGameObject(cloth);
				clothNodes.push_back(cloth);
			}
		}

		//add constraint between each node and the node below it
		for (int i = 0; i < clothNodes.size() - clothDimensions; i += clothDimensions) {
			for (int j = 0; j < clothDimensions; ++j) {
				DistanceConstraint* constraint = new DistanceConstraint(
					clothNodes.at(i + j)->Physics(),					//Physics Object A
					clothNodes.at(i + j + clothDimensions)->Physics(),					//Physics Object B
					clothNodes.at(i + j)->Physics()->GetPosition(),	//Attachment Position on Object A	-> Currently the centre
					clothNodes.at(i + j + clothDimensions)->Physics()->GetPosition());	//Attachment Position on Object B	-> Currently the centre  

				PhysicsEngine::Instance()->AddConstraint(constraint);
			}
		}

		//add constraint between each node and the node next to it
		for (int i = 0; i < clothNodes.size(); i += clothDimensions) {
			for (int j = 0; j < clothDimensions - 1; ++j) {
				DistanceConstraint* constraint = new DistanceConstraint(
					clothNodes.at(i + j)->Physics(),					//Physics Object A
					clothNodes.at(i + j + 1)->Physics(),					//Physics Object B
					clothNodes.at(i + j)->Physics()->GetPosition(),	//Attachment Position on Object A	-> Currently the centre
					clothNodes.at(i + j + 1)->Physics()->GetPosition());	//Attachment Position on Object B	-> Currently the centre  

				PhysicsEngine::Instance()->AddConstraint(constraint);
			}
		}

		//add constraint between each node and the node diagonally to it
		for (int i = 0; i < clothNodes.size() - clothDimensions; i += clothDimensions) {
			for (int j = 0; j < clothDimensions - 1; ++j) {
				DistanceConstraint* constraint = new DistanceConstraint(
					clothNodes.at(i + j)->Physics(),					//Physics Object A
					clothNodes.at(i + j + clothDimensions + 1)->Physics(),					//Physics Object B
					clothNodes.at(i + j)->Physics()->GetPosition(),	//Attachment Position on Object A	-> Currently the centre
					clothNodes.at(i + j + 1 + clothDimensions)->Physics()->GetPosition());	//Attachment Position on Object B	-> Currently the centre  

				PhysicsEngine::Instance()->AddConstraint(constraint);
			}
		}

		for (int i = 0; i < clothNodes.size() - clothDimensions; i += clothDimensions) {
			for (int j = 1; j < clothDimensions; ++j) {
				DistanceConstraint* constraint = new DistanceConstraint(
					clothNodes.at(i + j)->Physics(),					//Physics Object A
					clothNodes.at(i + j + clothDimensions - 1)->Physics(),					//Physics Object B
					clothNodes.at(i + j)->Physics()->GetPosition(),	//Attachment Position on Object A	-> Currently the centre
					clothNodes.at(i + j + clothDimensions - 1)->Physics()->GetPosition());	//Attachment Position on Object B	-> Currently the centre  

				PhysicsEngine::Instance()->AddConstraint(constraint);
			}
		}

	
	}

};