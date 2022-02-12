#pragma once

#include "RoomData.h"

namespace Gameplay
{
	class RoomLoader
	{
	public:	
		RoomLoader() {};

		void Initialize();
		void Add(std::string name, RoomData data);
		void LoadRoom(std::string name);
		void UnloadCurrentRoom();

	protected:
		std::map<std::string, RoomData> roomDictionary;
		RoomData currentRoom;

	};
}
