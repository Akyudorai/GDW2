#pragma once

#include "Scene.h"
#include "UI/GameInterfaceManager.h"

namespace Gameplay
{
	class GameManager
	{
	public:
		static GameManager& GetInstance() { static GameManager gameManager; return gameManager; }

	public:
		void Update(float deltaTime);
		void Reset();

		inline bool IsPaused() { return isPaused; }
		inline void SetPaused(bool state) { isPaused = state; }
		inline bool PlayerHasKey() { return hasKey; }
		inline void GiveKey() { hasKey = true; }
		inline float GetGameTime() { return gameTime; }

		inline Scene::Sptr GetScene() { return currentScene; }

	public:
		static GameInterfaceManager GameInterface;

	protected:
		static Scene::Sptr currentScene;
		static GLFWwindow* windowRef;

		bool hasKey = false;
		bool isPaused = false;
		float gameTime = 0.0f;
	};
}