#pragma once

#include "imgui.h"

namespace OMG 
{
	class DevTools
	{
		public:
			DevTools();
			~DevTools() = default;

			void Render();
		private:
			bool displayPanel = true;
	};
}