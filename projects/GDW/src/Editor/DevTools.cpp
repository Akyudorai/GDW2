
#include "DevTools.h"
#include "Hierarchy.h"
#include "../../NOU/include/NOU/App.h"
#include <stdlib.h>
using namespace nou;

namespace OMG
{
	

	void DevTools::Render() {
		
		// Draw the panel here	
		ImGui::SetNextWindowSize(ImVec2(1400, 700), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Editor", NULL, ImGuiWindowFlags_MenuBar))
		{
			// Menubar
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("New Scene")) { /* Do stuff */ }
					if (ImGui::MenuItem("Open Scene")) { /* Do stuff */ }
					if (ImGui::MenuItem("Save Scene")) { /* Do stuff */ }
					if (ImGui::MenuItem("Exit")) { exit(0); }

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Edit"))
				{
					if (ImGui::MenuItem("Undo")) { /* Do stuff */ }
					if (ImGui::MenuItem("Redo")) { /* Do stuff */ }
					if (ImGui::MenuItem("Cut")) { /* Do stuff */ }
					if (ImGui::MenuItem("Copy")) { /* Do stuff */ }
					if (ImGui::MenuItem("Paste")) { /* Do stuff */ }
					if (ImGui::MenuItem("Duplicate")) { /* Do stuff */ }
					if (ImGui::MenuItem("Rename")) { /* Do stuff */ }
					if (ImGui::MenuItem("Delete")) { /* Do stuff */ }					

					ImGui::EndMenu();
				}				

				if (ImGui::BeginMenu("Create"))
				{
					if (ImGui::MenuItem("Empty Object")) { /* Do stuff */ }
					if (ImGui::MenuItem("Prefab")) { /* Do stuff */ }
					if (ImGui::MenuItem("Camera")) { /* Do stuff */ }
					if (ImGui::MenuItem("Light")) { /* Do stuff */ }
					if (ImGui::MenuItem("Audio")) { /* Do stuff */ }
					if (ImGui::MenuItem("UI")) { /* Do stuff */ }	

					ImGui::EndMenu();
				}				

				if (ImGui::BeginMenu("Window"))
				{
					if (ImGui::MenuItem("Scene")) { /* Do stuff */ }
					if (ImGui::MenuItem("Game")) { /* Do stuff */ }
					if (ImGui::MenuItem("Hierarchy")) { /* Do stuff */ }
					if (ImGui::MenuItem("Inspector")) { /* Do stuff */ }					
					if (ImGui::MenuItem("Assets")) { /* Do stuff */ }
					if (ImGui::MenuItem("Console")) { /* Do stuff */ }

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			// Hierarchy			
			ImGui::BeginChild("Hierarchy", ImVec2(200, 500), true);			
			for (auto const& i : Hierarchy::GetInstance().entities) {
				//FIXME: Good candidate to use ImGuiSelectableFlags_SelectOnNav				
				if (ImGui::Selectable(i->m_name.c_str()))
				{
					// Do something with the selected item
				}
					
			}
			ImGui::EndChild();

			// Scene
			ImGui::SameLine();
			ImGui::BeginChild("Scene", ImVec2(965, 500), true);

			// Render the scene to this child frame
			
			ImGui::EndChild();


			// Inspector
			ImGui::SameLine();			
			ImGui::BeginChild("Inspector", ImVec2(200, 500), true);
				
			// Draw all the components that are attached to the selected object, including their modifiable variables

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