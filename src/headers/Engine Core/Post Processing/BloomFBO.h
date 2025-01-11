#ifndef _BLOOM_FBO_H_
#define _BLOOM_FBO_H_

#include <iostream>
#include <vector>
#include "../../Core Includes/GL_Core.h"
#include "../../Core Includes/GLM_Core.h"

struct BloomMip {
	glm::vec2 size{ 0 };
	glm::ivec2 intSize{ 0 };
	uint32_t texture{ 0 };
};

class BloomFBO {
private:
	uint32_t FBO{ 0 };
	bool isInit = false;
	std::vector<BloomMip> mMipChain{};

	uint32_t iterations = 6;

	void clearBuffers();

public:
	BloomFBO() = default;

	void resize(int width, int height);
	bool _init(int windowWidth, int windowHeight, uint32_t iterations = 6);

	const std::vector<BloomMip>& mipChain() const {
		return mMipChain;
	}

	void enableWriting() const {
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	}

	~BloomFBO();
};

#endif