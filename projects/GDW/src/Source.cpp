
#include "GameSceneManager.h"
#include "Editor/Debug.h"

using namespace GAME;
using namespace OMG;

int main()
{
	try {
		GameSceneManager::GetInstance()->Run();
	}
	catch (std::string fatalError) {
		Debug::LogError("Fatal Error: " + fatalError);		
	}
	catch (...) {
		Debug::LogError("Unknown Fatal Error");		
	}

	return 0;
}
