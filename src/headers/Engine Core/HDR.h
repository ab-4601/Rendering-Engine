#ifndef _HDR_H_
#define _HDR_H_

#include "../Core Includes/Core.h"
#include "Shader.h"
#include "Framebuffer.h"

class HDR final : public Framebuffer {
private:
	uint32_t colorBuffer = 0;
	uint32_t renderBuffer = 0;

	bool multisampled = false;
	int samples = 4;

	virtual void clearBuffers() override;

public:
	HDR() = default;

	HDR& operator=(const HDR&) = delete;
	static HDR& Framebuffer() {
		static HDR HDRFramebuffer{};
		return HDRFramebuffer; 
	}

	bool isMultisampled() const { return multisampled; }

	void enableMultisampling() { multisampled = true; }
	void disableMultisampling() { multisampled = false; }

	void _init(int width, int height);

	void updateSamplesMSAA(int width, int height, int samples) {
		if (!multisampled) return;

		this->samples = samples;
		width = std::max(1, width);
		height = std::max(1, height);

		clearBuffers();
		_init(width, height);
	}

	void resize(int width, int height) {
		width = std::max(1, width);
		height = std::max(1, height);

		clearBuffers();
		_init(width, height);
	}

	void copyDepthInfo(uint32_t depthFBO, int width, int height) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, depthFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	}
	
	uint32_t FramebufferID() const { return FBO; }
	uint32_t ColorbufferID() const { return colorBuffer; }

	void enableHDRWriting() const {
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	~HDR() {
		clearBuffers();
	}
};

#endif