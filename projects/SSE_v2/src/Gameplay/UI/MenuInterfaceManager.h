#pragma once

#include "Gameplay/GameObject.h"

using namespace Gameplay;

class GameInterfaceManager
{

public:
	GameObject* m_MainMenuPanel = nullptr;
	GameObject* m_OptionsMenuPanel = nullptr;
	GameObject* m_LevelSelectPanel = nullptr;
};