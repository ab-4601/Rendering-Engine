#ifndef _SELECTION_TEXTURE_H_
#define _SELECTION_TEXTURE_H_

#include <iostream>
#include "../Core Includes/GL_Core.h"

class SelectionTexture {
private:
	uint32_t FBO = 0;
	uint32_t selectionTexture = 0;
	uint32_t depthTexture = 0;
	uint32_t depthBuffer = 0;

public:
	SelectionTexture() = default;

	void init(int windowWidth, int windowHeight);

	void enableWriting() const { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO); }
	void disableWriting(GLuint framebuffer = 0) const { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer); }

	struct PixelInfo {
		uint32_t objectID = 0;
		uint32_t drawID = 0;
		uint32_t primitiveID = 0;
	};

	PixelInfo readPixel(int x, int y) const;

	~SelectionTexture();
};

#endif