#include "Framebuffer.h"
#include "Logging.h"

#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include <gl/GL.h>
#include <gl/GLU.h>


#include <GLM/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <GLM/gtx/common.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtc/matrix_transform.hpp>

Framebuffer::Framebuffer(int width, int height)
{
	// Generate Framebuffer
	glGenFramebuffers(1, &fboID);
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);

	// Create the texture to render the data to, and attach it to our Framebuffer
	this->texture = Texture(width, height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->texture.GetID(), 0);
	
	// Create renderBuffer to store the depth info
	GLuint rboID;
	glGenRenderbuffers(1, &rboID);
	glBindRenderbuffer(GL_RENDERBUFFER, rboID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboID);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		LOG_ERROR("Framebuffer is not complete");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}