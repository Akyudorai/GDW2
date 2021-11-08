#pragma once

#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <GLM/gtx/transform.hpp>
#include <vector>



class RenderingSystem
{


public:
	RenderingSystem(Window* window);
	~RenderingSystem();

	void Render();
	unsigned int getFrame() { return textureColorbuffer; }
	void setNewSize(int width, int height);

private:
	int shaderProgram;

	Window* window;
	unsigned int framebuffer, textureColorbuffer;
	int width, height;
	Shader* shader;

	// Projection and View matrix ID
	
};