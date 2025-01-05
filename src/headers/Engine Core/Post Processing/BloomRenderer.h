#ifndef _BLOOM_RENDERER_H_
#define _BLOOM_RENDERER_H_

#include "BloomFBO.h"
#include "../../Engine Core/Shader.h"
#include "../../Engine Core/Mesh/Geometry/Quad.h"

class BloomRenderer {
private:
	bool isInit = false;
	BloomFBO mFBO;

	uint32_t intermediateFBO{ 0 }, screenBuffer{ 0 };
	float knee{ 1.f };

	glm::ivec2 srcViewportSize{ 0 };
	glm::vec2 srcViewportSizeFLT{ 0.f };

	Shader upsampleShader{ "src/shaders/bloom.vert", "src/shaders/bloom_upsample.frag" };
	Shader downsampleShader{ "src/shaders/bloom.vert", "src/shaders/bloom_downsample.frag" };

	void renderDownsamples(uint32_t srcTexture);
	void renderUpsamples(float filterRadius);

	void _initIntermediateFBO(int width, int height);

	Quad quad;

public:
	BloomRenderer(int windowWidth, int windowHeight);

	inline void setKnee(float knee) { this->knee = (knee < 0.f) ? 0.f : knee; }

	bool _init(int windowWidth, int windowHeight);
	void renderBloomTexture(uint32_t srcTexture, float filterRadius, uint32_t currFramebuffer = 0);
	void renderBloomTextureMSAA(float filterRadius, uint32_t currFramebuffer = 0);

	GLuint bloomTexture() {
		return mFBO.mipChain()[0].texture;
	}

	~BloomRenderer() = default;
};

#endif