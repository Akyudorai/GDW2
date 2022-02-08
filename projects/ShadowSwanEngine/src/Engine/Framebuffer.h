#pragma once

#include "Engine/Texture.h"

class Framebuffer
{
private:
	GLuint fboID = 0;
	Texture texture;

public:
	Framebuffer() {}
	Framebuffer(int width, int height);

	GLuint GetFboID() {
		return fboID;
	}

	GLuint GetTextureID() {
		return texture.GetID();
	}

	void Bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, fboID);
	}

	void Unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


};