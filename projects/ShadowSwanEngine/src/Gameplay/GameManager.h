#pragma once

#include "CameraManager.h"

namespace Gameplay
{
	class GameManager
	{
	public:
		static GameManager& GetInstance() { static GameManager gameManager; return gameManager; }
		
	public:
		void Initialize();
		void Update(float deltaTime);
		void Reset();
		

		bool PlayerHasKey() { return hasKey; }
		void GiveKey() { hasKey = true; }
		float GetGameTime() { return gameTime; }

	public:
		CameraManager cameraManager;

	protected:
		bool hasKey = false;
		bool isPaused = false;
		float gameTime = 0.0f;
		

	};
}