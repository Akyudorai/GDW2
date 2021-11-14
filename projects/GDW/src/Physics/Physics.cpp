#include "Physics.h"
using namespace MATH;

#include "../../NOU/include/NOU/App.h"

bool Physics::SphereSphereCollision(const SphereCollider& c1, const SphereCollider& c2) 
{
	glm::vec3 center1 = c1.m_owner->transform.m_pos;
	glm::vec3 center2 = c2.m_owner->transform.m_pos;
	float dist = VMath::distance(center1, center2);
	if (dist < (c1.r + c2.r)) {
		return true;
	}

	return false;
}

void Physics::SphereSphereCollisionResponse(SphereCollider& c1, SphereCollider& c2)
{	
	glm::vec3 direction = c1.m_owner->transform.m_pos - c2.m_owner->transform.m_pos;
	glm::vec3 normalized = VMath::normalize(direction);

	//c1.m_owner->Get<PhysicsObject>().velocity -= c1.m_owner->Get<PhysicsObject>().velocity;
	//c2.m_owner->Get<PhysicsObject>().velocity -= c2.m_owner->Get<PhysicsObject>().velocity;
	if (!c1.isTrigger && !c2.isTrigger) {
		//Debug::Log("Collision Event");
		c1.m_owner->transform.m_pos -= c1.m_owner->Get<PhysicsObject>().velocity * 0.1f;
		c2.m_owner->transform.m_pos -= c2.m_owner->Get<PhysicsObject>().velocity * 0.1f;
	}	
	
	else if (c1.isTrigger || c2.isTrigger) {
		//Debug::Log("Trigger Event");
		if (c1.onTriggerEvent) c1.onTriggerEvent();
		if (c2.onTriggerEvent) c2.onTriggerEvent();
	}

}

bool Physics::BoxBoxCollision(const BoxCollider& c1, const BoxCollider& c2)
{
	glm::vec3 p1 = c1.m_owner->transform.m_pos;
	glm::vec3 p2 = c2.m_owner->transform.m_pos;
	glm::vec3 e1 = c1.m_extents;
	glm::vec3 e2 = c2.m_extents;

	if (
		p1.x + e1.x/2 > p2.x - e2.x/2 &&
		p1.x - e1.x/2 < p2.x + e2.x/2 &&
		p1.y + e1.y/2 > p2.y - e2.y/2 &&
		p1.y - e1.y/2 < p2.y + e2.y/2 &&
		p1.z + e1.z/2 > p2.z - e2.z/2 &&
		p1.z - e1.z/2 < p2.z + e2.z/2 ) {
		return true;
	}

	return false;
}


// Change it so that it checks the collision before movement. If a collision is detected, cancel the movement.
// If not, continue the movement.  This way we don't have to keep pushing the entity back, preventing buggy collision.
void Physics::BoxBoxCollisionResponse(BoxCollider& c1, BoxCollider& c2)
{
	glm::vec3 direction = c1.m_owner->transform.m_pos - c2.m_owner->transform.m_pos;
	glm::vec3 normalized = VMath::normalize(direction);

	if (!c1.isTrigger && !c2.isTrigger) {		
			
		if (c1.m_owner->Has<PhysicsObject>()) {
			c1.m_owner->transform.m_pos -= c1.m_owner->Get<PhysicsObject>().velocity * 0.025f;
		}		
		if (c2.m_owner->Has<PhysicsObject>()) {
			c2.m_owner->transform.m_pos -= c2.m_owner->Get<PhysicsObject>().velocity * 0.025f;
		}
				
	}

	else if (c1.isTrigger && !c2.isTrigger || !c1.isTrigger && c2.isTrigger) {
		if (c1.onTriggerEvent) c1.onTriggerEvent();
		if (c2.onTriggerEvent) c2.onTriggerEvent();
	}
}

bool Physics::SphereBoxCollision(const SphereCollider& c1, const BoxCollider& c2)
{
	return false;
}



