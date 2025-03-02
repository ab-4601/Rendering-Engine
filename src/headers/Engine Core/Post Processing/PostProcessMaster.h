#ifndef _POST_PROCESS_MASTER_H_
#define _POST_PROCESS_MASTER_H_

#include "BloomRenderer.h"
#include "../../Core Includes/GL_Core.h"
#include "../Mesh/Geometry/Quad.h"
#include "../HDR.h"

class PostProcessMaster {
private:
	uint32_t FBO{ 0 }, RBO{ 0 }, colorBuffer{ 0 };
	uint32_t intermediateFBO{ 0 }, screenBuffer{ 0 };

	glm::ivec4 screenDimensions{ 0 };

	uint32_t edgeFBO = 0, blendFBO = 0;
	uint32_t edgeColorBuffer = 0, blendColorBuffer = 0;

	Quad quad;

	BloomRenderer bloom;

	Shader shader{ "src/shaders/post_process.vert", "src/shaders/post_process.frag" };
	Shader edgeShader{ "src/shaders/smaa.vert", "src/shaders/smaa_edge.frag" };
	Shader blendShader{ "src/shaders/smaa.vert", "src/shaders/smaa_blend.frag" };

	void _init(int width, int height) const;
	void _initIntermediateFBO(int width, int height) const;

	void _initSMAA(int width, int height);
	void calcSMAAbuffers(uint32_t screenTexture);

	void clearBuffers();

public:
	PostProcessMaster(int windowWidth, int windowHeight);

	PostProcessMaster& operator=(const PostProcessMaster&) = delete;
	static PostProcessMaster& Framebuffer() {
		static PostProcessMaster PostProcess{ 800, 600 };
		return PostProcess; 
	}

	uint32_t ColorbufferID() const { return colorBuffer; }
	uint32_t FramebufferID() const { return FBO; }

	void enableWriting() const { 
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void resize(glm::ivec4 viewportDimensions, bool isMSAA = false) {
		int width = std::max(1, viewportDimensions.z);
		int height = std::max(1, viewportDimensions.w);

		screenDimensions = viewportDimensions;

		bloom.resize(viewportDimensions);
		_init(width, height);

		if(isMSAA)
			_initIntermediateFBO(width, height);
	}

	void postProcessingPass(bool applyBloom, float threshold, float knee, float filterRadius, float exposure);
	void postProcessingPassMSAA(bool applyBloom, float threshold, float knee, float filterRadius, float exposure);

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