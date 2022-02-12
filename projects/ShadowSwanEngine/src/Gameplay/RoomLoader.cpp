#include "RoomLoader.h"
#include "GameManager.h"

namespace Gameplay
{
	void RoomLoader::Initialize()
	{	
		// Unload all Rooms
		for (std::map<std::string, RoomData>::iterator it = roomDictionary.begin(); it != roomDictionary.end(); ++it)
		{
			for (int i = 0; i < it->second.ObjectsInRoom.size(); i++)
			{
				it->second.ObjectsInRoom[i]->SetActive(false);
			}			
		}

		// Load first room
		LoadRoom(roomDictionary.begin()->first);
	}

	void RoomLoader::Add(std::string name, RoomData data)
	{		
		roomDictionary.emplace(name, data);
	}

	void RoomLoader::LoadRoom(std::string name)
	{
		// Unload Current Room			
		UnloadCurrentRoom();
			
		// Set Current Room
		currentRoom = roomDictionary[name];
		GameManager::GetInstance().cameraManager.SetRoomData(&currentRoom);
	
		// Load the Room
		for (int i = 0; i < roomDictionary[name].ObjectsInRoom.size(); i++)
		{			
			// Enable objects and their components
			currentRoom.ObjectsInRoom[i]->SetActive(true);						
		}
	}

	void RoomLoader::UnloadCurrentRoom()
	{
		// Unload the current room
		for (int i = 0; i < currentRoom.ObjectsInRoom.size(); i++)
		{
			// Disable object and its components
			currentRoom.ObjectsInRoom[i]->SetActive(false);			
		}
	}
}