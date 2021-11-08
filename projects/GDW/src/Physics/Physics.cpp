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

void Physics::SphereSphereCollisionResponse(SphereCollider& c1, const SphereCollider& c2)
{
	float e = 0.05f;
	glm::vec3 direction = c1.m_owner->transform.m_pos - c2.m_owner->transform.m_pos;
	glm::vec3 normalized = VMath::normalize(direction);

	//c1.m_owner->Get<PhysicsObject>().velocity -= c1.m_owner->Get<PhysicsObject>().velocity;
	//c2.m_owner->Get<PhysicsObject>().velocity -= c2.m_owner->Get<PhysicsObject>().velocity;
	if (!c1.isTrigger && !c2.isTrigger) {
		Debug::Log("Collision Event");
		c1.m_owner->transform.m_pos -= c1.m_owner->Get<PhysicsObject>().velocity * 0.1f;
		c2.m_owner->transform.m_pos -= c2.m_owner->Get<PhysicsObject>().velocity * 0.1f;
	}	
	
	else if (c1.isTrigger || c2.isTrigger) {
		Debug::Log("Trigger Event");
	}

}

