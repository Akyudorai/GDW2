#include "GameManager.h"

namespace Gameplay
{
	GameInterfaceManager GameManager::GameInterface = GameInterfaceManager();

	void GameManager::Update(float deltaTime)
	{
		// Increment Game Timer
		if (!isPaused) {
			gameTime += deltaTime;
		}
	}

	void GameManager::Reset()
	{
		hasKey = false;
		gameTime = 0.0f;
	}
}