#ifndef _SELECTION_TEXTURE_H_
#define _SELECTION_TEXTURE_H_

#include <iostream>
#include "../../Core Includes/GL_Core.h"

struct PixelInfo {
	uint32_t objectID = 0;
	uint32_t drawID = 0;
	uint32_t primitiveID = 0;
};

class SelectionTexture {
private:
	uint32_t FBO = 0;
	uint32_t selectionTexture = 0;
	uint32_t depthTexture = 0;

	void clearBuffers();

public:
	SelectionTexture() = default;

	void init(int windowWidth, int windowHeight);

	void resize(int width, int height) {
		width = std::max(1, width);
		height = std::max(1, height);

		clearBuffers();
		init(width, height);
	}

	uint32_t getSelectionColorBuffer() const { return selectionTexture; }

	void enableWriting() const { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO); }
	void disableWriting(GLuint framebuffer = 0) const { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer); }

	PixelInfo readPixel(int x, int y) const;

	~SelectionTexture();
};

#endif