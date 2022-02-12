#include "GameManager.h"

namespace Gameplay
{
	void GameManager::Initialize()
	{
		// Set Initial Values
		hasKey = false;
		gameTime = 0.0f;

		// Initialize Camera Manager
		cameraManager = CameraManager();
	}

	void GameManager::Update(float deltaTime)
	{
		// Increment Game Timer
		if (!isPaused) {
			gameTime += deltaTime;
		}

		// Manage Camera
		cameraManager.Update(deltaTime);
	}

	void GameManager::Reset()
	{
		hasKey = false;
		gameTime = 0.0f;
	}
}