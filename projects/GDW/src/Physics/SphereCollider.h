#pragma once

#include "GLM/glm.hpp"
#include "../../NOU/include/NOU/Entity.h"
using namespace nou;

class SphereCollider
{
public:	
	Entity* m_owner;
	float r;
	bool isTrigger = false;

	std::function<void()> onTriggerEvent;

protected:
	

public:
	SphereCollider(Entity& owner, float radius)
	{
		m_owner = &owner;
		r = radius;
	}
	virtual ~SphereCollider() = default;

	SphereCollider(SphereCollider&&) = default;
	SphereCollider& operator=(SphereCollider&&) = default;

	
};