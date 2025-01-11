#ifndef _POST_PROCESS_MASTER_H_
#define _POST_PROCESS_MASTER_H_

#include "BloomRenderer.h"
#include "../../Core Includes/GL_Core.h"
#include "../Mesh/Geometry/Quad.h"

class PostProcessMaster {
private:
	uint32_t FBO{ 0 }, colorBuffer{ 0 };
	uint32_t intermediateFBO{ 0 }, screenBuffer{ 0 };

	glm::ivec2 screenDimensions{ 0 };

	uint32_t edgeFBO = 0, blendFBO = 0;
	uint32_t edgeColorBuffer = 0, blendColorBuffer = 0;

	Quad quad;

	BloomRenderer bloom;

	Shader shader{ "src/shaders/post_process.vert", "src/shaders/post_process.frag" };
	Shader edgeShader{ "src/shaders/smaa.vert", "src/shaders/smaa_edge.frag" };
	Shader blendShader{ "src/shaders/smaa.vert", "src/shaders/smaa_blend.frag" };

	void _init(int width, int height);
	void _initSMAA(int width, int height);
	void _initIntermediateFBO(int width, int height);
	void calcSMAAbuffers(uint32_t screenTexture, int width, int height);

	void clearBuffers();

public:
	PostProcessMaster(int windowWidth, int windowHeight);

	uint32_t getColorBuffer() const { return colorBuffer; }

	void resizeBuffers(int width, int height) {
		clearBuffers();

		bloom.resize(width, height);
		_init(width, height);
		_initIntermediateFBO(width, height);
	}

	void postProcessingPass(uint32_t hdrBuffer, bool applyBloom, float knee, float filterRadius, float exposure);
	void postProcessingPassMSAA(uint32_t hdrFBO, bool applyBloom, float knee, float filterRadius, float exposure);

	void blitToDefaultFramebuffer() const {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(
			0, 0, screenDimensions.x, screenDimensions.y,
			0, 0, screenDimensions.x, screenDimensions.y,
			GL_COLOR_BUFFER_BIT, GL_LINEAR
		);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	~PostProcessMaster() {
		clearBuffers();
	}
};

#endif