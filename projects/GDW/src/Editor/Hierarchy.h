#pragma once

#include <list>
#include "NOU/Entity.h"
#include "imgui.h"
#include <string>

using namespace nou;

namespace OMG 
{
	class Hierarchy
	{
	public:
		static Hierarchy& GetInstance() {
			static Hierarchy instance;
			return instance;
		}
	
		void AddEntity(Entity* entity);
		void RemoveEntity(Entity* entity);

		Entity* GetEntity(int index);
		int GetIndex(Entity* e);

		std::list<Entity*> entities;

	private:		
		Hierarchy() {};
		Hierarchy(Hierarchy const&) = delete;
		void operator=(Hierarchy const&) = delete;

		
		bool displayPanel = true;

	};
}


