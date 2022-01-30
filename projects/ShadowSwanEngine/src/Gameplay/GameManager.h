#pragma once

namespace Gameplay
{
	class GameManager
	{
	public:
		static GameManager& GetInstance() { static GameManager gameManager; return gameManager; }
		
	public:
		void Update(float deltaTime);
		void Reset();

		bool PlayerHasKey() { return hasKey; }
		void GiveKey() { hasKey = true; }
		float GetGameTime() { return gameTime; }

	protected:
		bool hasKey = false;
		bool isPaused = false;
		float gameTime = 0.0f;


	};
}