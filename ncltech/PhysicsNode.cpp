#include "PhysicsNode.h"
#include "PhysicsEngine.h"


void PhysicsNode::IntegrateForVelocity(float dt)
{
	
		//gravity
		if (invMass > 0.0f) {
			linVelocity += PhysicsEngine::Instance()->GetGravity() * dt;
		}

		//semi implicit euler
		//TODO replace with rk4 if time
		linVelocity += force * invMass * dt;

		//velocity damping
		linVelocity = linVelocity * PhysicsEngine::Instance()->GetDampingFactor();

		//angular rotation
		angVelocity += invInertia * torque * dt;

		//angular velocity damping
		angVelocity = angVelocity *  PhysicsEngine::Instance()->GetDampingFactor();
	
}

/* Between these two functions the physics engine will solve for velocity
   based on collisions/constraints etc. So we need to integrate velocity, solve 
   constraints, then use final velocity to update position. 
*/

void PhysicsNode::IntegrateForPosition(float dt)
{
	
		Vector3	oldPos = position;
		//update the position
		position += linVelocity * dt;

		//update orientation
		orientation = orientation + Quaternion(angVelocity * dt * 0.5f, 0.0f) * orientation;

		orientation.Normalise();

		if (oldPos == position) {
			atRest = true;
		}
		//Finally: Notify any listener's that this PhysicsNode has a new world transform.
		// - This is used by GameObject to set the worldTransform of any RenderNode's. 
		//   Please don't delete this!!!!!
		FireOnUpdateCallback();
	
}