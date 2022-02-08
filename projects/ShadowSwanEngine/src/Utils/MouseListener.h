#pragma once

#include "GLFW/glfw3.h"
#include <iostream>

class MouseListener
{
public:
	static MouseListener& GetInstance() { static MouseListener listener; return listener; }

private:
	double scrollX, scrollY;
	double xPos, yPos, lastX, lastY;
	bool mouseButtonPressed[3];
	bool isDragging = false;

public:
	static void MousePositionCallback(long window, double xPos, double yPos) {
		GetInstance().lastX = GetInstance().xPos;
		GetInstance().lastY = GetInstance().yPos;
		GetInstance().xPos = xPos;
		GetInstance().yPos = yPos;
		GetInstance().isDragging = GetInstance().mouseButtonPressed[0] || GetInstance().mouseButtonPressed[1] || GetInstance().mouseButtonPressed[2];
	}

	static void MouseButtonCallback(long window, int button, int action, int mods) {
		if (action == GLFW_PRESS) {
			if (button < (sizeof(GetInstance().mouseButtonPressed) / sizeof(GetInstance().mouseButtonPressed[0]))) {
				GetInstance().mouseButtonPressed[button] = true;
			}			
		} else if (action == GLFW_RELEASE) {
			if (button < (sizeof(GetInstance().mouseButtonPressed) / sizeof(GetInstance().mouseButtonPressed[0]))) {
				GetInstance().mouseButtonPressed[button] = false;
				GetInstance().isDragging = false;
			}
		}
	}

	static void MouseScrollCallback(long window, double xOffset, double yOffset) {
		GetInstance().scrollX = xOffset;
		GetInstance().scrollY = yOffset;
	}

	static void EndFrame() {
		GetInstance().scrollX = 0;
		GetInstance().scrollY = 0;
		GetInstance().lastX = GetInstance().xPos;
		GetInstance().lastY = GetInstance().yPos;
	}

	static float GetX() {
		return (float)GetInstance().xPos;
	}

	static float GetY() {
		return (float)GetInstance().yPos;
	}

	static float GetDx() {
		return (float)(GetInstance().lastX - GetInstance().xPos);
	}

	static float GetDy() {
		return (float)(GetInstance().lastY - GetInstance().yPos);
	}

	static float GetScrollX() {
		return (float)(GetInstance().scrollX);
	}
	
	static float GetScrollY() {
		return (float)(GetInstance().scrollY);
	}

	static bool IsDragging() {
		return GetInstance().isDragging;
	}

	static bool MouseButtonDown(int button) {
		if (button < (sizeof(GetInstance().mouseButtonPressed) / sizeof(GetInstance().mouseButtonPressed[0]))) {
			return GetInstance().mouseButtonPressed[button];
		} else {
			false;
		}
	}

private:
	MouseListener();

};