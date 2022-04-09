#pragma once

#include "Scene.h"
#include "UI/GameInterfaceManager.h"
#include "Gameplay/PlayerController.h"

#include <vector>

namespace Gameplay
{
	struct Highscore
	{
	public:
		float CompletionTime = 0;
	
	public:
		Highscore(float val) {
			CompletionTime = val;
		}
	};

	class Leaderboard
	{
	private:
		std::vector<Highscore> HighScores = std::vector<Highscore>();

	public:
		Leaderboard();
		void Push(Highscore score);
		void Sort();
		void Debug();

		void Save();
		void Load();
	};

	class GameManager
	{
	public:
		static GameManager& GetInstance() { static GameManager gameManager; return gameManager; }

	public:
		void Update(float deltaTime);
		void Reset();

		inline bool IsMainMenu() { return isMainMenu; }
		inline void SetIsMainMenu(bool state) { isMainMenu = state; }
		inline bool IsGameOver() { return isGameOver; }
		inline void SetGameOver(bool state) { isGameOver = state; }
		inline bool IsPaused() { return isPaused; }
		inline void SetPaused(bool state) { isPaused = state; }
		inline bool PlayerHasKey() { return hasKey; }
		inline void GiveKey() { hasKey = true; }
		inline float GetGameTime() { return gameTime; }
		inline void SetPC(PlayerController::Sptr controller) { pc = controller; }
		inline PlayerController::Sptr GetPC() { return pc; }

	public:
		static GameInterfaceManager GameInterface;
		static Leaderboard leaderboard;

	protected:
		PlayerController::Sptr pc;
		bool hasKey = false;
		bool isPaused = false;
		float gameTime = 0.0f;
		bool isGameOver = false;
		bool isMainMenu = false;
	};
}