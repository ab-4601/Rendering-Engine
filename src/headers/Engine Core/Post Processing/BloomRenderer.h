#ifndef _BLOOM_RENDERER_H_
#define _BLOOM_RENDERER_H_

#include "BloomFBO.h"
#include "../../Engine Core/Shader.h"
#include "../../Application/AttriblessRendering.h"

class BloomRenderer {
private:
	bool isInit = false;
	BloomFBO mFBO;

	uint32_t intermediateFBO{ 0 }, screenBuffer{ 0 };
	float threshold{ 0.f };

	glm::ivec4 srcViewportSize{ 0 };
	glm::vec2 srcViewportSizeFLT{ 0.f };

	Shader upsampleShader{ "src/shaders/bloom.vert", "src/shaders/bloom_upsample.frag" };
	Shader downsampleShader{ "src/shaders/bloom.vert", "src/shaders/bloom_downsample.frag" };

	void downsample(uint32_t srcTexture);
	void upsample(float filterRadius);

	void _initIntermediateFBO(int width, int height);

public:
	BloomRenderer() = default;

	void resize(glm::ivec4 viewportDimensions) {
		int width = std::max(1, viewportDimensions.z);
		int height = std::max(1, viewportDimensions.w);

		srcViewportSize = viewportDimensions;
		srcViewportSizeFLT = glm::vec2((float)width, (float)height);

		isInit = false;
		mFBO.resize(width, height);
	}

	void setThreshold(float threshold) { this->threshold = (threshold < 0.f) ? 0.f : threshold; }

	bool _init(int windowWidth, int windowHeight);
	void renderBloomTexture(uint32_t srcTexture, float filterRadius);

	GLuint bloomTexture() {
		return mFBO.mipChain()[0].texture;
	}

	~BloomRenderer() = default;
};

#endif