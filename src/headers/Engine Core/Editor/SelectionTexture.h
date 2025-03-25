#ifndef _SELECTION_TEXTURE_H_
#define _SELECTION_TEXTURE_H_

#include "../Framebuffer.h"

struct PixelInfo {
	uint32_t objectID = 0;
	uint32_t drawID = 0;
	uint32_t primitiveID = 0;
};

class SelectionTexture final : public Framebuffer {
private:
	uint32_t selectionTexture = 0;
	uint32_t depthTexture = 0;

	virtual void clearBuffers() override;

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

	void enableWriting() const { 
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void disableWriting(GLuint framebuffer = 0) const { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer); }

	PixelInfo readPixel(int x, int y) const;

	~SelectionTexture();
};

#endif