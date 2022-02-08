#pragma once

#include "GLM/glm.hpp"
#include "glad/glad.h"
#include <string>

class Texture
{
private:
	std::string filePath;
	GLuint texID;
	int width, height;

public:
	Texture() {}
	Texture(int width, int height);

	void Bind() {
		glBindTexture(GL_TEXTURE_2D, texID);
	}

	void Unbind() {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	int GetWidth() {
		return width;
	}

	int GetHeight() {
		return height;
	}

	GLuint GetID() {
		return texID;
	}

};