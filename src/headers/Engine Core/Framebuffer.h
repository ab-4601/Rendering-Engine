#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include <iostream>
#include <glad/glad.h>

class Framebuffer {
protected:
	uint32_t FBO = 0;
	bool checkFramebufferStatus();

public:
	virtual void clearBuffers() = 0;

	~Framebuffer() = default;
};

#endif