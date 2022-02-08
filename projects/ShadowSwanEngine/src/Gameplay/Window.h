#pragma once

#include "GLFW/glfw3.h"
#include <string>
#include "GLM/glm.hpp"
#include "Scene.h"
#include "Engine/Framebuffer.h"


class Window
{
private:	
	static Gameplay::Scene currentScene;

	GLFWwindow* glfwWindow;
	int width, height;
	std::string title;
	bool fadeToBlade = false;
	Framebuffer framebuffer;

public:
	float r,g,b,a;

private:
	Window();

public:
	static Window& GetInstance() { static Window window; return window; }
	static Gameplay::Scene GetScene() { return currentScene; }
	static int GetWidth() { return GetInstance().width; }
	static int GetHeight() { return GetInstance().height; }
	static void SetWidth(int newWidth) { GetInstance().width = newWidth; }
	static void SetHeight(int newHeight) { GetInstance().height = newHeight;  }
	static GLFWwindow* GetWindow() { return GetInstance().glfwWindow; }
	static Framebuffer GetFramebuffer() {
		return GetInstance().framebuffer;
	}
	static float GetTargetAspectRatio() {
		return 16.0f / 9.0f;
	}

	void Init();
	void Run();
	void Loop();
	void ChangeScene(int index);
};