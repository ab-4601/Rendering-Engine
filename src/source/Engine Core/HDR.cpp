#include "../../headers/Engine Core/HDR.h"

void HDR::clearBuffers() {
	if (FBO != 0) {
		glDeleteFramebuffers(1, &FBO);
		glDeleteTextures(1, &colorBuffer);
		glDeleteRenderbuffers(1, &renderBuffer);

		FBO = 0;
		colorBuffer = 0;
		renderBuffer = 0;
	}
}

void HDR::_init(int width, int height) {
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &colorBuffer);

	if (multisampled) {
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorBuffer);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA16F, width, height, GL_TRUE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colorBuffer, 0);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, colorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
	}

	glGenRenderbuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);

	if (multisampled) 
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
	else 
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
	
	if (!checkFramebufferStatus()) {
		std::cerr << "HDR framebuffer initialization error" << std::endl;
		exit(-1);
	}

	GLenum target = (multisampled) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

	glBindTexture(target, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}