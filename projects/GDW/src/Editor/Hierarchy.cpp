#include "Hierarchy.h"

#include <iostream>


namespace OMG 
{

	void Hierarchy::Render() 
	{
		ImGui::Begin("Hierarchy", &displayPanel, ImVec2(300, 700));

		// Draw the Panel
		ImGui::Text("Hierarchy Test");

		if (ImGui::Button("Create Object")) {
			Entity newObject = Entity::Create("New Object");
			//newObject.m_name = "New Object";
			AddEntity(&newObject);
		}
		
		for (auto const& i : entities) {
			
			if (ImGui::Button(const_cast<char*>(i->m_name.c_str()), ImVec2(285, 20))) {

			}
		}

		ImGui::End();
	}

	void Hierarchy::AddEntity(Entity* entity) 
	{
		std::cout << "Entity has been added" << std::endl;
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