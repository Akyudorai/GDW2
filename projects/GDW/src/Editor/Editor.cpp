
#include "Editor.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "../../NOU/include/NOU/App.h"
#include <stdlib.h>
<<<<<<< HEAD:projects/GDW/src/Editor/DevTools.cpp
=======
#include "Utilities.h"
>>>>>>> ECS-Systems:projects/GDW/src/Editor/Editor.cpp
using namespace nou;

using namespace Editor;

namespace OMG
{
	void Editor::Render() {
		
		// Draw the panel here	
		ImGui::SetNextWindowSize(ImVec2(1400, 700), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Editor", NULL, ImGuiWindowFlags_MenuBar))
		{
			// Menubar
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(125, 125, 125, 255));
					if (ImGui::MenuItem("New Scene")) { /* Do stuff */ }
					if (ImGui::MenuItem("Open Scene")) { /* Do stuff */ }
					if (ImGui::MenuItem("Save Scene")) { /* Do stuff */ }
<<<<<<< HEAD:projects/GDW/src/Editor/DevTools.cpp
					if (ImGui::MenuItem("Exit")) { exit(0); }
=======
					ImGui::PopStyleColor();
					
					if (ImGui::MenuItem("Exit")) 
					{ 
						glfwSetWindowShouldClose(App::GetWindow(), 1);
					}
					
>>>>>>> ECS-Systems:projects/GDW/src/Editor/Editor.cpp

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Edit"))
				{
					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(125, 125, 125, 255));
					if (ImGui::MenuItem("Undo")) { /* Do stuff */ }
					if (ImGui::MenuItem("Redo")) { /* Do stuff */ }
					if (ImGui::MenuItem("Cut")) { /* Do stuff */ }
					if (ImGui::MenuItem("Copy")) { /* Do stuff */ }
					if (ImGui::MenuItem("Paste")) { /* Do stuff */ }
					if (ImGui::MenuItem("Duplicate")) { /* Do stuff */ }
										
					if (ImGui::MenuItem("Delete"))
					{
						if (selectedEntity != NULL)
						{
							// Destroy the selected object
						}
					}
					ImGui::PopStyleColor();

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Create"))
				{
					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(125, 125, 125, 255));
					if (ImGui::MenuItem("Empty Object")) 
					{						
						// The issue that is occurring is that the object is created in the current frame and the pointer is saved in hierarchy, not the object itself.
						// So when the next frame arrives, the object no longer exists and we have a pointer to something that doesnt exist anymore.
						// EDIT :: I tried restructuring the code to use list<Entity> instead of list<Entity*> and ran into an error I could solve within 5 hours.
						//		I have returned back to using pointer reference list rather than an object reference list.

						//Entity::Create("New Object");
					}
					if (ImGui::MenuItem("Prefab")) { /* Do stuff */ }
					if (ImGui::MenuItem("Camera")) { /* Do stuff */ }
					if (ImGui::MenuItem("Light")) { /* Do stuff */ }
					if (ImGui::MenuItem("Audio")) { /* Do stuff */ }
					if (ImGui::MenuItem("UI")) { /* Do stuff */ }
					ImGui::PopStyleColor();

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Window"))
				{
					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(125, 125, 125, 255));
					if (ImGui::MenuItem("Scene")) { /* Do stuff */ }
					if (ImGui::MenuItem("Game")) { /* Do stuff */ }
					if (ImGui::MenuItem("Hierarchy")) { /* Do stuff */ }
					if (ImGui::MenuItem("Inspector")) { /* Do stuff */ }
					if (ImGui::MenuItem("Assets")) { /* Do stuff */ }
					if (ImGui::MenuItem("Console")) { /* Do stuff */ }
					ImGui::PopStyleColor();

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			// Hierarchy				

			ImGui::BeginChild("Hierarchy", ImVec2(200, 500), true);

			for (auto const& e : Hierarchy::GetInstance().entities)
			{
				int index = Hierarchy::GetInstance().GetIndex(e);
				if (ImGui::Selectable(const_cast<char*>(e->m_name.c_str()), selectedEntity == index))
				{
					selectedEntity = index;
				}
			}

			ImGui::EndChild();
		
			

			// Scene
			ImGui::SameLine();
			ImGui::BeginChild("Scene", ImVec2(865, 500), true);

			// Render the scene to this child frame
			
			ImGui::EndChild();


			// Inspector
			ImGui::SameLine();			
			ImGui::BeginChild("Inspector", ImVec2(300, 500), true);
				
			// Draw all the components that are attached to the selected object, including their modifiable variables
			if (selectedEntity != NULL)
			{
				Entity* selected = Hierarchy::GetInstance().GetEntity(selectedEntity);				
				Inspector::GetInstance().Render(selected);
			}

			ImGui::EndChild();

			// Assets
			ImGui::BeginChild("Assets", ImVec2(1380, 0), true);

			// Draw the file path for assets


			ImGui::EndChild();
				
		}

		

			
		//if (ImGui::BeginMenuBar()) 
		//{
		//	if (ImGui::BeginMenu("File"))
		//	{
		//		if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
		//		if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
		//		if (ImGui::MenuItem("Close", "Ctrl+W")) { /* Do stuff */ }
		//		ImGui::EndMenu();				
		//	}

		//	ImGui::EndMenuBar();
		//}

		//// Edit a color (stored as 4 floats)
		//ImGui::ColorEdit4("Color", my_color);
		//
		//// Plot some values
		//const float my_values[] = { 0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f };
		//ImGui::PlotLines("Frame Times", my_values, IM_ARRAYSIZE(my_values));

		//// Display Contents in a scrolling region
		//ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
		//ImGui::BeginChild("Scrolling");
		//for (int n = 0; n < 50; n++)
		//	ImGui::Text("%04d: Some text", n);
		//ImGui::EndChild();

		ImGui::End();
	}
}