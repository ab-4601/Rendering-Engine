#ifndef _POST_PROCESS_MASTER_H_
#define _POST_PROCESS_MASTER_H_

#include "BloomRenderer.h"
#include "../../Application/AttriblessRendering.h"
#include "../HDR.h"

class PostProcessMaster final : public Framebuffer {
private:
	uint32_t RBO{ 0 }, colorBuffer{ 0 };
	uint32_t intermediateFBO{ 0 }, blitColorBuffer{ 0 };

	glm::ivec4 screenDimensions{ 0 };

	uint32_t edgeFBO = 0, blendFBO = 0;
	uint32_t edgeColorBuffer = 0, blendColorBuffer = 0;

	BloomRenderer bloom;

	Shader shader{ "src/shaders/post_process.vert", "src/shaders/post_process.frag" };
	Shader edgeShader{ "src/shaders/smaa.vert", "src/shaders/smaa_edge.frag" };
	Shader blendShader{ "src/shaders/smaa.vert", "src/shaders/smaa_blend.frag" };

	void _initIntermediateFBO(int width, int height);
	void _initSMAA(int width, int height);

	void calcSMAAbuffers(uint32_t screenTexture);

	virtual void clearBuffers() override;

public:
	PostProcessMaster(int windowWidth, int windowHeight) {
		bloom._init(windowWidth, windowHeight);
		screenDimensions = glm::ivec4(0, 0, windowWidth, windowHeight);
	}

	PostProcessMaster& operator=(const PostProcessMaster&) = delete;
	static PostProcessMaster& Framebuffer() {
		static PostProcessMaster PostProcess{ 800, 600 };
		return PostProcess; 
	}

	uint32_t ColorbufferID() const { return colorBuffer; }
	uint32_t FramebufferID() const { return FBO; }

	void _init(int width, int height);

	void enableWriting() const { 
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void resize(glm::ivec4 viewportDimensions) {
		int width = std::max(1, viewportDimensions.z);
		int height = std::max(1, viewportDimensions.w);

		screenDimensions = viewportDimensions;

		clearBuffers();

		bloom.resize(viewportDimensions);
		_init(width, height);
	}

	void postProcessingPass(bool applyBloom, float threshold, float knee, float filterRadius, float exposure);

	void blitToDefaultFramebuffer() const {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(
			screenDimensions.x, screenDimensions.y, screenDimensions.z, screenDimensions.w,
			screenDimensions.x, screenDimensions.y, screenDimensions.z, screenDimensions.w,
			GL_COLOR_BUFFER_BIT, GL_NEAREST
		);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	~PostProcessMaster() {
		clearBuffers();
	}
};

#endif