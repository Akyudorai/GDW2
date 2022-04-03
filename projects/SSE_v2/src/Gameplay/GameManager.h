#pragma once

#include "Scene.h"
#include "UI/GameInterfaceManager.h"
#include "Gameplay/PlayerController.h"

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
		inline void SetPC(PlayerController::Sptr controller) { pc = controller; }
		inline PlayerController::Sptr GetPC() { return pc; }

	public:
		static GameInterfaceManager GameInterface;

	protected:
		PlayerController::Sptr pc;
		bool hasKey = false;
		bool isPaused = false;
		float gameTime = 0.0f;
	};
}