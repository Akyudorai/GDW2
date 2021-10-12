#pragma once

#include "imgui.h"

namespace OMG 
{
	class DevTools
	{
	public:
		static DevTools& GetInstance() {
			static DevTools instance;
			return instance;
		}

		DevTools Initialize();
		void Render();


	private:
		DevTools() {};
		DevTools(DevTools const&) = delete;
		void operator=(DevTools const&) = delete;

		bool displayPanel = true;		

	};
}