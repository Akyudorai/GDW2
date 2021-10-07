
#include "DevTools.h"

namespace OMG
{
	DevTools::DevTools() 
	{

	}

	void DevTools::Render() {
		
		ImGui::Begin("Development Tools", &displayPanel, ImVec2(300, 200));

		// Draw the panel here
		if (ImGui::Button("Test")) {

		}

		ImGui::End();
	}
}