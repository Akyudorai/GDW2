#pragma once

#include "imgui.h"

namespace OMG 
{
	class Editor
	{
	public:
		static Editor& GetInstance() {
			static Editor instance;
			return instance;
		}

		Editor Initialize();
		void Render();


	private:
		Editor() {};
		Editor(Editor const&) = delete;
		void operator=(Editor const&) = delete;

		bool displayPanel = true;	
		int selectedEntity = 0;		

	};
}