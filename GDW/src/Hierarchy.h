#pragma once

#include <list>
#include "NOU/Entity.h"
#include "imgui.h"

using namespace nou;

namespace OMG 
{
	class Hierarchy
	{
	public:
		static Hierarchy& getInstance() {
			static Hierarchy instance;
			return instance;
		}

		Hierarchy Initialize();
		void Render();
		void AddEntity(Entity* entity);
		void RemoveEntity(Entity* entity);

	private:		
		Hierarchy() {};
		Hierarchy(Hierarchy const&) = delete;
		void operator=(Hierarchy const&) = delete;

		std::list<Entity*> entities;
		bool displayPanel = true;

	};
}


