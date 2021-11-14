#pragma once

#include "GLM/glm.hpp"
#include "../../NOU/include/NOU/Entity.h"

#include "Physics/Vector.h"

using namespace nou;

class BoxCollider
{
public:
	Entity* m_owner;
	glm::vec3 vertices[8];

	glm::vec3 m_position;
	glm::vec3 m_extents;

	bool isTrigger = false;
	std::function<void()> onTriggerEvent;

protected:

	void Initialize() 
	{
		glm::vec3 p = m_position;
		glm::vec3 e = m_extents;

		/* Vertex Pts
		0 - Left/Bottom/Back = Vec3(p-e.x, p-e.y, p-e.z)
		1 - Right/Bottom/Back = Vec3(p+e.x, p-e.y, p-e.z)
		2 - Left/Top/Back = Vec3(p-e.x, p+e.y, p-e.z)
		3 - Right/Top/Back = Vec3(p+e.x, p+e.y, p-e.z)
		4 - Left/Bottom/Front = Vec3(p-e.x, p-e.y, p+e.z)
		5 - Right/Bottom/Front = Vec3(p+e.x, p-e.y, p+e.z)
		6 - Left/Top/Front = Vec3(p-e.x, p+e.y, p+e.z)
		7 - Right/Top/Front = Vec3(p+e.x, p+e.y, p+e.z)
		*/

		vertices[0] = glm::vec3(p.x - e.x, p.y - e.y, p.z - e.z);
		vertices[1] = glm::vec3(p.x + e.x, p.y - e.y, p.z - e.z);
		vertices[2] = glm::vec3(p.x - e.x, p.y + e.y, p.z - e.z);
		vertices[3] = glm::vec3(p.x + e.x, p.y + e.y, p.z - e.z);
		vertices[4] = glm::vec3(p.x - e.x, p.y - e.y, p.z + e.z);
		vertices[5] = glm::vec3(p.x + e.x, p.y - e.y, p.z + e.z);
		vertices[6] = glm::vec3(p.x - e.x, p.y + e.y, p.z + e.z);
		vertices[7] = glm::vec3(p.x + e.x, p.y + e.y, p.z + e.z);

		/* Planes
		0 - Left - 
		1 - Right - 
		2 - Top - 
		3 - Bottom - 
		4 - Back - 
		5 - Front - 
		
		*/

	}

public:
	BoxCollider(Entity& owner, glm::vec3 extents)
	{
		m_owner = &owner;
		m_extents = extents;
		m_position = owner.transform.m_pos;					
		
		for (int i = 0; i < 8; i++)
		{
			vertices[i] = glm::vec3(0, 0, 0);
		}


		Initialize();
	}

	virtual ~BoxCollider() = default;
	BoxCollider(BoxCollider&&) = default;
	BoxCollider& operator=(BoxCollider&&) = default;
};