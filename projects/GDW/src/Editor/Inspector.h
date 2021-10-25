#pragma once

#include "../../NOU/include/NOU/Entity.h"

using namespace nou;

namespace Editor
{
	class Inspector
	{
	private:
		enum ObjectType {
			Transform
		};

		

	public:
		static Inspector& GetInstance() {
			static Inspector instance;
			return instance;
		}
		
		void Render(Entity* object);


	

	};
}