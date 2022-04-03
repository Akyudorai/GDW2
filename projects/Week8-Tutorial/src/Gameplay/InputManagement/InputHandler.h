#pragma once

#include "NOU/Input.h"
#include "../SceneManagement/SceneManager.h"

class InputHandler
{
public:
	
	static InputHandler& shared_instance() { static InputHandler inputHandler; return inputHandler; }
	std::map<int, bool> key_state;
	std::map<int, bool> old_key_state;
	
	static void Initialize()
	{
		shared_instance().key_state.emplace(GLFW_KEY_W, false);
		shared_instance().key_state.emplace(GLFW_KEY_S, false);
		shared_instance().key_state.emplace(GLFW_KEY_A, false);
		shared_instance().key_state.emplace(GLFW_KEY_D, false);
		shared_instance().key_state.emplace(GLFW_KEY_Q, false);
		shared_instance().key_state.emplace(GLFW_KEY_E, false);
		shared_instance().key_state.emplace(GLFW_KEY_F, false);
		shared_instance().key_state.emplace(GLFW_KEY_R, false);
		shared_instance().key_state.emplace(GLFW_KEY_P, false);
		shared_instance().key_state.emplace(GLFW_KEY_TAB, false);
		shared_instance().key_state.emplace(GLFW_KEY_F1, false);
		shared_instance().key_state.emplace(GLFW_KEY_F2, false);
		shared_instance().key_state.emplace(GLFW_KEY_F3, false);
		shared_instance().key_state.emplace(GLFW_KEY_F4, false);
		shared_instance().key_state.emplace(GLFW_KEY_F5, false);
		shared_instance().key_state.emplace(GLFW_KEY_F6, false);
		shared_instance().key_state.emplace(GLFW_KEY_SPACE, false);
	}

	static void Update(float deltaTime)
	{
		shared_instance().old_key_state = shared_instance().key_state;

		for (std::map<int, bool>::iterator it = shared_instance().key_state.begin(); it != shared_instance().key_state.end(); ++it)
		{
			if (glfwGetKey(SceneManager::GetCurrentScene()->Window, it->first) == GLFW_PRESS && it->second == false) {
				it->second = true;
			}
			else if (glfwGetKey(SceneManager::GetCurrentScene()->Window, it->first) == GLFW_RELEASE && it->second == true) {
				it->second = false;
			}
		}
	}

	

	static bool GetKeyDown(int key)
	{
		bool state = shared_instance().GetOldKeyState(key);
		bool newState = shared_instance().GetKeyState(key);

		if (state == false && newState == true) {
			LOG_WARN("Key Down: " + std::to_string(key) + ".");
			return true;
		};
		return false;
	}

	static bool GetKeyUp(int key)
	{
		bool state = shared_instance().GetOldKeyState(key);
		bool newState = shared_instance().GetKeyState(key);

		if (state == true && newState == false) {
			LOG_WARN("Key Up: " + std::to_string(key) + ".");
			return true;
		}
		return false;		
	}

	static bool GetKey(int key)
	{
		return shared_instance().GetKeyState(key);
	}

private:
	bool GetKeyState(int key) {
		std::map<int, bool>::iterator it;
		it = key_state.find(key);

		if (it != key_state.end())
		{
			return it->second;
		}

		else {
			std::cout << "ERROR <nullptr>: " << key << " not found in Key States.";
			return false;
		}
	}

	bool GetOldKeyState(int key) {
		std::map<int, bool>::iterator it;
		it = old_key_state.find(key);

		if (it != old_key_state.end())
		{
			return it->second;
		}

		else {
			std::cout << "ERROR <nullptr>: " << key << " not found in Old Key States.";
			return false;
		}
	}
	

	//static bool SetKeyState(int key, bool state)
	//{
	//	std::map<int, bool>::iterator it;
	//	it = shared_instance().key_state.find(key);

	//	if (it != shared_instance().key_state.end()) {
	//		it->second = state;
	//	}

	//	else {
	//		shared_instance().key_state.emplace(key, state);
	//	}

	//	return state;
	//}
};