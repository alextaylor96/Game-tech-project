#include "PhysicsEngine.h"
#include "GameObject.h"
#include "CollisionDetectionSAT.h"
#include <nclgl\NCLDebug.h>
#include <nclgl\Window.h>
#include <omp.h>
#include <algorithm>
#include "Octree.h"


Octree* tree = nullptr;

void PhysicsEngine::SetDefaults()
{
	//Variables set here /will/ be reset with each scene
	updateTimestep = 1.0f / 60.f;
	updateRealTimeAccum = 0.0f;
	gravity = Vector3(0.0f, -9.81f, 0.0f);
	dampingFactor = 0.999f;
}

PhysicsEngine::PhysicsEngine()
{
	//Variables set here will /not/ be reset with each scene
	isPaused = false;  
	debugDrawFlags = DEBUGDRAW_FLAGS_MANIFOLD | DEBUGDRAW_FLAGS_CONSTRAINT;
	
	SetDefaults();

	tree = new Octree(&physicsNodes);
}

PhysicsEngine::~PhysicsEngine()
{
	RemoveAllPhysicsObjects();
}

void PhysicsEngine::AddPhysicsObject(PhysicsNode* obj)
{
	physicsNodes.push_back(obj);
}

void PhysicsEngine::RemovePhysicsObject(PhysicsNode* obj)
{
	//Lookup the object in question
	auto found_loc = std::find(physicsNodes.begin(), physicsNodes.end(), obj);

	//If found, remove it from the list
	if (found_loc != physicsNodes.end())
	{
		physicsNodes.erase(found_loc);
	}
}

void PhysicsEngine::RemoveAllPhysicsObjects()
{
	//Delete and remove all constraints/collision manifolds
	for (Constraint* c : constraints)
	{
		delete c;
	}
	constraints.clear();

	for (Manifold* m : manifolds)
	{
		delete m;
	}
	manifolds.clear();


	//Delete and remove all physics objects
	// - we also need to inform the (possibly) associated game-object
	//   that the physics object no longer exists
	for (PhysicsNode* obj : physicsNodes)
	{
		if (obj->GetParent()) obj->GetParent()->SetPhysics(NULL);
		delete obj;
	}

	//delete the existing tree
	physicsNodes.clear();
}


void PhysicsEngine::Update(float deltaTime)
{
	//The physics engine should run independantly to the renderer
	// - As our codebase is currently single threaded we just need
	//   a way of calling "UpdatePhysics()" at regular intervals
	//   or multiple times a frame if the physics timestep is higher
	//   than the renderers.
	const int max_updates_per_frame = 5;

	if (!isPaused)
	{
		updateRealTimeAccum += deltaTime;
		for (int i = 0; (updateRealTimeAccum >= updateTimestep) && i < max_updates_per_frame; ++i)
		{
			updateRealTimeAccum -= updateTimestep;

			//Additional IsPaused check here incase physics was paused inside one of it's components for debugging or otherwise
			if (!isPaused) UpdatePhysics(); 
		}

		if (updateRealTimeAccum >= updateTimestep)
		{
			NCLDebug::Log("Physics too slow to run in real time!");
			//Drop Time in the hope that it can continue to run faster the next frame
			updateRealTimeAccum = 0.0f;
		}
	}
}


void PhysicsEngine::UpdatePhysics()
{
	for (Manifold* m : manifolds)
	{
		delete m;
	}
	manifolds.clear();

	perfUpdate.UpdateRealElapsedTime(updateTimestep);
	perfBroadphase.UpdateRealElapsedTime(updateTimestep);
	perfNarrowphase.UpdateRealElapsedTime(updateTimestep);
	perfSolver.UpdateRealElapsedTime(updateTimestep);
	 
	tree->update();
	
	
	
	//A whole physics engine in 6 simple steps =D
	
	//-- Using positions from last frame --
//1. Broadphase Collision Detection (Fast and dirty)
	perfBroadphase.BeginTimingSection();
	BroadPhaseCollisions();
	perfBroadphase.EndTimingSection();

//2. Narrowphase Collision Detection (Accurate but slow)
	perfNarrowphase.BeginTimingSection();
	NarrowPhaseCollisions();
	perfNarrowphase.EndTimingSection();

	std::random_shuffle(manifolds.begin(), manifolds.end());
	std::random_shuffle(constraints.begin(), constraints.end());

//3. Initialize Constraint Params (precompute elasticity/baumgarte factor etc)
	//Optional step to allow constraints to 
	// precompute values based off current velocities 
	// before they are updated loop below.

	for (Manifold* m : manifolds) m->PreSolverStep(updateTimestep);

	for (Constraint* c : constraints) c->PreSolverStep(updateTimestep);

	
//4. Update Velocities
	perfUpdate.BeginTimingSection();
	for (PhysicsNode* obj : physicsNodes) obj->IntegrateForVelocity(updateTimestep);
	perfUpdate.EndTimingSection();

//5. Constraint Solver
	perfSolver.BeginTimingSection();
	for (Constraint* c : constraints) c->ApplyImpulse();
	perfSolver.EndTimingSection();

	for (size_t i = 0; i < SOLVER_ITERATIONS; ++i) {
		for (Manifold* m : manifolds) m->ApplyImpulse();
		for (Constraint* c : constraints) c->ApplyImpulse();
	}
	
//6. Update Positions (with final 'real' velocities)
	perfUpdate.BeginTimingSection();
	for (PhysicsNode* obj : physicsNodes) obj->IntegrateForPosition(updateTimestep);
	perfUpdate.EndTimingSection();
}

void PhysicsEngine::BroadPhaseCollisions()
{
	broadphaseColPairs.clear();

	PhysicsNode *pnodeA, *pnodeB;
	//	The broadphase needs to build a list of all potentially colliding objects in the world,
	//	which then get accurately assesed in narrowphase. If this is too coarse then the system slows down with
	//	the complexity of narrowphase collision checking, if this is too fine then collisions may be missed.


	//do rest broadphase here




	//octree to build pairs
	//maybe ask about this because kind of doesnt speed up much
	vector<vector<PhysicsNode*>> possiblePairs = tree->getPossibleCollisions();

	for (int k = 0; k < possiblePairs.size(); ++k)
	{
		if (possiblePairs.at(k).size() > 0)
		{
			for (size_t i = 0; i < possiblePairs.at(k).size() - 1; ++i)
			{
				for (size_t j = i + 1; j < possiblePairs.at(k).size(); ++j)
				{
					pnodeA = possiblePairs.at(k)[i];
					pnodeB = possiblePairs.at(k)[j];

					//Check they both atleast have collision shapes and atleast one isnt at rest
					if (pnodeA->GetCollisionShape() != NULL
						&& pnodeB->GetCollisionShape() != NULL
						//causes problems with floor collision
						/*&& (!pnodeA->getRestState() || !pnodeB->getRestState())*/)
					{
						CollisionPair cp;
						cp.pObjectA = pnodeA;
						cp.pObjectB = pnodeB;

						broadphaseColPairs.push_back(cp);
					}
				}
			}
		}
	}
}


void PhysicsEngine::NarrowPhaseCollisions()
{
	if (broadphaseColPairs.size() > 0)
	{
		//Collision data to pass between detection and manifold generation stages.
		CollisionData colData;				

		//Collision Detection Algorithm to use
		CollisionDetectionSAT colDetect;	

		// Iterate over all possible collision pairs and perform accurate collision detection
		for (size_t i = 0; i < broadphaseColPairs.size(); ++i)
		{
			CollisionPair& cp = broadphaseColPairs[i];

			CollisionShape *shapeA = cp.pObjectA->GetCollisionShape();
			CollisionShape *shapeB = cp.pObjectB->GetCollisionShape();

			colDetect.BeginNewPair(
				cp.pObjectA,
				cp.pObjectB,
				cp.pObjectA->GetCollisionShape(),
				cp.pObjectB->GetCollisionShape());

			//--TUTORIAL 4 CODE--
			// Detects if the objects are colliding
			if (colDetect.AreColliding(&colData))
			{
				//Note: As at the end of tutorial 4 we have very little to do, this is a bit messier
				//      than it should be. We now fire oncollision events for the two objects so they
				//      can handle AI and also optionally draw the collision normals to see roughly
				//      where and how the objects are colliding.

				//Draw collision data to the window if requested
				// - Have to do this here as colData is only temporary. 
				if (debugDrawFlags & DEBUGDRAW_FLAGS_COLLISIONNORMALS)
				{
					NCLDebug::DrawPointNDT(colData._pointOnPlane, 0.1f, Vector4(0.5f, 0.5f, 1.0f, 1.0f));
					NCLDebug::DrawThickLineNDT(colData._pointOnPlane, colData._pointOnPlane - colData._normal * colData._penetration, 0.05f, Vector4(0.0f, 0.0f, 1.0f, 1.0f));
				}

				//Check to see if any of the objects have a OnCollision callback that dont want the objects to physically collide
				bool okA = cp.pObjectA->FireOnCollisionEvent(cp.pObjectA, cp.pObjectB);
				bool okB = cp.pObjectB->FireOnCollisionEvent(cp.pObjectB, cp.pObjectA);

				cp.pObjectA->setRestState(false);
				cp.pObjectB->setRestState(false);

				if (okA && okB)
				{
					Manifold* manifold = new Manifold();
					manifold->Initiate(cp.pObjectA, cp.pObjectB);
					colDetect.GenContactPoints(manifold);

					if (manifold->contactPoints.size() > 0) {
						manifolds.push_back(manifold);
					}
					else {
						delete manifold;
					}
				}
			}
		}

	}
}


void PhysicsEngine::DebugRender()
{
	tree->root->DebugDraw();
	// Draw all collision manifolds
	if (debugDrawFlags & DEBUGDRAW_FLAGS_MANIFOLD)
	{
		for (Manifold* m : manifolds)
		{
			m->DebugDraw();
		}
	}

	// Draw all constraints
	if (debugDrawFlags & DEBUGDRAW_FLAGS_CONSTRAINT)
	{
		for (Constraint* c : constraints)
		{
			c->DebugDraw();
		}
	}

	// Draw all associated collision shapes
	if (debugDrawFlags & DEBUGDRAW_FLAGS_COLLISIONVOLUMES)
	{
		for (PhysicsNode* obj : physicsNodes)
		{
			if (obj->GetCollisionShape() != NULL)
			{
				obj->GetCollisionShape()->DebugDraw();
			}
		}
	}
}