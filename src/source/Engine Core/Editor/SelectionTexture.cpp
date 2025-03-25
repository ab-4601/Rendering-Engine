#include "../../../headers/Engine Core/Editor/SelectionTexture.h"

void SelectionTexture::init(int windowWidth, int windowHeight) {
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &selectionTexture);
	glBindTexture(GL_TEXTURE_2D, selectionTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, windowWidth, windowHeight, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, selectionTexture, 0);

	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

	if (!checkFramebufferStatus()) {
		std::cerr << "Framebuffer error. Exiting program" << std::endl;
		exit(1);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SelectionTexture::clearBuffers() {
	if (FBO != 0) {
		glDeleteFramebuffers(1, &FBO);
		FBO = 0;
	}

	if (selectionTexture != 0) {
		glDeleteTextures(1, &selectionTexture);
		selectionTexture = 0;
	}

	if (depthTexture != 0) {
		glDeleteTextures(1, &depthTexture);
		depthTexture = 0;
	}
}

PixelInfo SelectionTexture::readPixel(int x, int y) const {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	PixelInfo pixel;
	glReadPixels(x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &pixel);

	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	return pixel;
}

SelectionTexture::~SelectionTexture() {
	clearBuffers();
}