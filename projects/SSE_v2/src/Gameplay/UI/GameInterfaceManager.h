#pragma once

#include "Gameplay/GameObject.h"
#include "Gameplay/Components/GUI/GuiText.h"

using namespace Gameplay;

class GameInterfaceManager
{
public:
	GameInterfaceManager()
	{ }

	void Clear()
	{
		m_bodyHealthDisplay = nullptr;
		m_shadowHealthDisplay = nullptr;

		m_GameUserInterface = nullptr;
		m_PauseMenuPanel = nullptr;
		m_OptionsMenuPanel = nullptr;
		m_LosePanel = nullptr;
		m_WinPanel = nullptr;
	}

public:

	void SetGameUserInterface(GameObject& panel) {
		m_GameUserInterface = &panel;
	}

	void InitializeGameUserInterface(GuiText& bodyHealthDisplay, GuiText& shadowHealthDisplay) {
		m_bodyHealthDisplay = &bodyHealthDisplay;
		m_shadowHealthDisplay = &shadowHealthDisplay;
	}

	void ToggleGameUserInterface(bool state) {
		m_GameUserInterface->IsActive = state;
	}

	void SetPausePanel(GameObject& panel) {
		m_PauseMenuPanel = &panel;
	}

	void TogglePausePanel(bool state) {
		m_PauseMenuPanel->IsActive = state;
	}

	void SetOptionsPanel(GameObject& panel) {
		m_OptionsMenuPanel = &panel;
	}

	void ToggleOptionsPanel(bool state) {
		m_OptionsMenuPanel->IsActive = state;
	}

	void SetLosePanel(GameObject& panel) {
		m_LosePanel = &panel;
	}

	void ToggleLosePanel(bool state) {
		m_LosePanel->IsActive = state;
	}

	void SetWinPanel(GameObject& panel) {
		m_WinPanel = &panel;
	}

	void ToggleWinPanel(bool state) {
		m_WinPanel->IsActive = state;
	}

public:
	GuiText* m_bodyHealthDisplay = nullptr;
	GuiText* m_shadowHealthDisplay = nullptr;

	GameObject* m_GameUserInterface = nullptr;
	GameObject* m_PauseMenuPanel = nullptr;
	GameObject* m_OptionsMenuPanel = nullptr;
	GameObject* m_LosePanel = nullptr;
	GameObject* m_WinPanel = nullptr;
};