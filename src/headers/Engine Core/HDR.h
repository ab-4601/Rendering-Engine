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
	uint32_t depthBuffer = 0;

	uint32_t intermediateFBO = 0;
	uint32_t screenBuffer = 0;

	uint32_t edgeFBO = 0, blendFBO = 0;
	uint32_t edgeColorBuffer = 0, blendColorBuffer = 0;

	Texture areaTexture{ "src/Content/Textures/SMAA/AreaTexDX10.dds" },
		searchTexture{ "src/Content/Textures/SMAA/SearchTex.dds" };

	glm::ivec2 screenResolution;

	Shader shader{ "src/shaders/hdr.vert", "src/shaders/hdr.frag" };
	Shader edgeShader{ "src/shaders/smaa.vert", "src/shaders/smaa_edge.frag" };
	Shader blendShader{ "src/shaders/smaa.vert", "src/shaders/smaa_blend.frag" };

	Quad quad;

	bool checkFramebufferStatus();

	void _initIntermediateFBO();
	void _initSMAAbuffers();

	void calcSMAAbuffers(uint32_t screenTexture);

public:
	HDR(int windowWidth = 800, int windowHeight = 800);

	void _init();
	void _initMSAA();

	uint32_t getFramebufferID() const { return FBO; }
	uint32_t getColorbufferID() const { return colorBuffer; }

	void enableHDRWriting() const { glBindFramebuffer(GL_FRAMEBUFFER, FBO); }

	void renderToDefaultBuffer(float exposure = 1.f, uint32_t bloomBuffer = 0, bool enableBloom = false);
	void renderToDefaultBufferMSAA(float exposure = 1.f, uint32_t bloomBuffer = 0, bool enableBloom = false);

	~HDR();
};

#endif