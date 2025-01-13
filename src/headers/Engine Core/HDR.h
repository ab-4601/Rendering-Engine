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

	void _init(int width, int height);
	void _initMSAA(int width, int height);

	void resize(int width, int height, bool isMSAA = false) {
		clearBuffers();

		if (isMSAA) 
			_initMSAA(width, height);
		else
			_init(width, height);
	}

	uint32_t getFramebufferID() const { return FBO; }
	uint32_t getColorbufferID() const { return colorBuffer; }

	void enableHDRWriting() const { glBindFramebuffer(GL_FRAMEBUFFER, FBO); }

	~HDR() {
		clearBuffers();
	}
};

#endif