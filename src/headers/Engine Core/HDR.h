#ifndef _HDR_H_
#define _HDR_H_

#include "../Core Includes/Core.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh/Geometry/Quad.h"

class HDR {
private:
	uint32_t FBO = 0;
	uint32_t colorBuffer = 0;
	uint32_t renderBuffer = 0;

	Quad quad;

	bool checkFramebufferStatus();
	void clearBuffers();

public:
	HDR();

	HDR& operator=(const HDR&) = delete;
	static HDR& Framebuffer() {
		static HDR HDRFramebuffer{};
		return HDRFramebuffer; 
	}

	void _init(int width, int height);
	void _initMSAA(int width, int height, int samples = ::samples);

	void updateSamplesMSAA(int width, int height, int samples) {
		if (width <= 0 || height <= 0) return;

		clearBuffers();
		_initMSAA(width, height, samples);
	}

	void resize(int width, int height, bool isMSAA = false) {
		if (width <= 0 || height <= 0) {
			std::cerr << "Invalid framebuffer dimensions" << std::endl;
			return;
		}

		clearBuffers();

		if (isMSAA) 
			_initMSAA(width, height);
		else
			_init(width, height);
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