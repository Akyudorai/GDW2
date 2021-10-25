#include "Hierarchy.h"

#include <iostream>


namespace OMG 
{

	void Hierarchy::AddEntity(Entity* entity) 
	{
		std::cout << "Entity Created: " << entity->m_name << std::endl;
		entities.push_back(entity);
	}

	void Hierarchy::RemoveEntity(Entity* entity)
	{
		entities.remove(entity);
	}

	Entity* Hierarchy::GetEntity(int index)
	{
		int i = 0;
		std::list<Entity*>::iterator it;
		for (it = entities.begin(); it != entities.end(); ++it)
		{
			i++;

			if (i == index) {
				return *it;
			}
		}		
	}

	int Hierarchy::GetIndex(Entity* e)
	{
		int i = 0;
		std::list<Entity*>::iterator it;
		for (it = entities.begin(); it != entities.end(); ++it)
		{
			i++;

			if (*it == e) {
				return i;
			}
		}
	}
}