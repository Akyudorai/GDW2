
#include "Inspector.h"
#include "imgui.h"
#include <string>
#include <cstring>

#include "NOU/CCamera.h"
#include "NOU/CMeshRenderer.h"
#include "Resources.h"
#include "NOU/Mesh.h"

#include <iostream>

using namespace std;
using namespace OMG;

namespace Editor
{	
	void Inspector::Render(Entity* const object) {

		ImGui::BeginChild("Scrolling");

		// Render Name
		int n_length = object->m_name.length();
		char n_buf[64];
		strcpy(n_buf, object->m_name.c_str());
		
		ImGui::InputText("Name", n_buf, IM_ARRAYSIZE(n_buf));
		if (object->m_name != n_buf) {
			object->m_name = n_buf;
		}
		

		// Render Transform
		ImGui::Separator();
		ImGui::Text("Transform");
		ImGui::InputFloat3("Position", (float*)&object->transform.m_pos, 5);
		ImGui::InputFloat3("Rotation", (float*)&object->transform.m_rotation, 5);
		ImGui::InputFloat3("Scale", (float*)&object->transform.m_scale, 5);

		// Render Other Components
		if (object->Has<CCamera>())
		{
			ImGui::Separator();
			ImGui::Text("Camera");

			//// Camera Mode			
			//if (ImGui::BeginCombo("Mode", "Select Mode")) 
			//{
			//	if (ImGui::Selectable("Perspective", false))
			//	{
			//		// Change to Perspective and set default values
			//		object->Get<CCamera>().Perspective(60.0f, 1.0f, 0.1f, 100.0f);
			//	}

			//	if (ImGui::Selectable("Orthographic", false))
			//	{
			//		// Change to Orthographic and set default values
			//		object->Get<CCamera>().Ortho(5, 5, 5, 5, 0.1f, 100.0f);
			//	}

			//	ImGui::EndCombo();
			//}
			//
			//if (object->Get<CCamera>().mode == CCamera::CameraMode::Perspective) 
			//{
			//	// FoV (Perspective)
			//	ImGui::SliderFloat("FoV", (float*)&object->Get<CCamera>().fov, 0, 360);
			//	// Aspect (Perspective)
			//	ImGui::InputFloat("Aspect", (float*)&object->Get<CCamera>().aspect);
			//}

			//else if (object->Get<CCamera>().mode == CCamera::CameraMode::Orthographic) 
			//{
			//	// Left (Ortho)
			//	ImGui::InputFloat("Left", (float*)&object->Get<CCamera>().left);
			//	// Right (Ortho)
			//	ImGui::InputFloat("Right", (float*)&object->Get<CCamera>().right);
			//	// Bottom (Ortho)
			//	ImGui::InputFloat("Bottom", (float*)&object->Get<CCamera>().bottom);
			//	// Top (ortho)
			//	ImGui::InputFloat("Top", (float*)&object->Get<CCamera>().top);
			//}
			//

			//// Near Plane
			//ImGui::InputFloat("Near Plane", (float*)&object->Get<CCamera>().nearPlane);
			//
			//// Far Plane
			//ImGui::InputFloat("Far Plane", (float*)&object->Get<CCamera>().farPlane);
			//
			//if (object->Get<CCamera>().mode == CCamera::CameraMode::Perspective)
			//{
			//	object->Get<CCamera>().Perspective(, 1.0f, 0.1f, 100.0f);
			//}

			//if (ImGui::Selectable("Orthographic", false))
			//{
			//	// Change to Orthographic and set default values
			//	object->Get<CCamera>().Ortho(5, 5, 5, 5, 0.1f, 100.0f);
			//}
		}

		if (object->Has<CMeshRenderer>())
		{
			ImGui::Separator();
			ImGui::Text("Mesh Renderer");

			// Mesh
			const char* preview = "Select Mesh";			
			if (ImGui::BeginCombo("Mesh", preview))
			{
				for (std::map<string, unique_ptr<Mesh>>::iterator it = Resources::GetInstance().Meshes.begin(); it != Resources::GetInstance().Meshes.end(); ++it)
				{
					if (ImGui::Selectable(const_cast<char*>(it->first.c_str()), false)) {
						object->Get<CMeshRenderer>().SetMesh(*Resources::GetInstance().Meshes[it->first]);
					}
				}

				ImGui::EndCombo();
			}

			// Color
			ImGui::InputFloat3("Color", (float*)&object->Get<CMeshRenderer>().GetMaterial()->m_color, 5);

		}

		ImGui::EndChild();
	}


}