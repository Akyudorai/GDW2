/*
NOU Framework - Created for INFR 2310 at Ontario Tech.
(c) Samantha Stahlke 2020

Entity.cpp
Minimal and lightly OOP wrapper for the ENTT framework.
*/

#include "NOU/Entity.h"

namespace nou
{
	entt::registry Entity::ecs;

	Entity Entity::Create(std::string name)
	{			
		entt::entity id = ecs.create();		
		return Entity(id, name);
	}

	std::unique_ptr<Entity> Entity::Allocate(std::string name)
	{
		entt::entity id = ecs.create();
		return std::move(std::make_unique<Entity>(id, name));
	}

	Entity::Entity(entt::entity id, std::string name)
	{
		m_id = id;
		m_name = name;
	}

	Entity::~Entity()
	{
		if(m_id != entt::null)
			ecs.destroy(m_id);
	}
}