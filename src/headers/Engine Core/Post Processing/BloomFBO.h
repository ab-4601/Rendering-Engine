#ifndef _BLOOM_FBO_H_
#define _BLOOM_FBO_H_

#include <vector>
#include "../../Core Includes/GLM_Core.h"
#include "../Framebuffer.h"

struct BloomMip {
	glm::vec2 size{ 0 };
	glm::ivec2 intSize{ 0 };
	uint32_t texture{ 0 };
};

class BloomFBO final : public Framebuffer {
private:
	bool isInit = false;
	std::vector<BloomMip> mMipChain{};

	uint32_t iterations = 5;

	virtual void clearBuffers() override;

public:
	BloomFBO() = default;

	void resize(int width, int height);
	bool _init(int windowWidth, int windowHeight, uint32_t iterations = 5);

	const std::vector<BloomMip>& mipChain() const {
		return mMipChain;
	}

	void enableWriting() const {
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	}

	~BloomFBO();
};

#endif