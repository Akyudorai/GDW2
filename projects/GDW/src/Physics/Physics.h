#pragma once

#include "VMath.h"
#include "SphereCollider.h"
#include "BoxCollider.h"

class Physics {
public:
	static bool SphereSphereCollision(const SphereCollider& c1, const SphereCollider& c2);
	static void SphereSphereCollisionResponse(SphereCollider& c1, SphereCollider& c2);
	static bool BoxBoxCollision(const BoxCollider& c1, const BoxCollider& c2);
	static void BoxBoxCollisionResponse(BoxCollider& c1, BoxCollider& c2);
	static bool SphereBoxCollision(const SphereCollider& c1, const BoxCollider& c2);
	static void SphereBoxCollisionResponse(SphereCollider& c1, BoxCollider& c2);
	
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