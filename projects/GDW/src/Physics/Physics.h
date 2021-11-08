#pragma once

#include "VMath.h"
#include "SphereCollider.h"


class Physics {
public:
	static bool SphereSphereCollision(const SphereCollider& c1, const SphereCollider& c2);
	static void SphereSphereCollisionResponse(SphereCollider& c1, const SphereCollider& c2);
};

class PhysicsObject {
public:
	Entity* m_owner;
	glm::vec3 velocity = glm::vec3(0);

	void Update(float deltaTime) {
		m_owner->transform.m_pos += velocity * deltaTime;
	}

public:
	PhysicsObject(Entity& owner)
	{
		m_owner = &owner;
	}

	virtual ~PhysicsObject() = default;
	PhysicsObject(PhysicsObject&&) = default;
	PhysicsObject& operator=(PhysicsObject&&) = default;
};