#include "../../../headers/Engine Core/Post Processing/PostProcessMaster.h"

PostProcessMaster::PostProcessMaster(int windowWidth, int windowHeight) {
	bloom._init(windowWidth, windowHeight);

	screenDimensions = glm::ivec4(0, 0, windowWidth, windowHeight);
	quad.createQuad();

	_init(windowWidth, windowHeight);
	_initIntermediateFBO(windowWidth, windowHeight);
}

void PostProcessMaster::clearBuffers() {
	if (FBO != 0) {
		glDeleteFramebuffers(1, &FBO);
		glDeleteRenderbuffers(1, &RBO);
		glDeleteTextures(1, &colorBuffer);

		FBO = 0; RBO = 0;
		colorBuffer = 0;
	}

	if (intermediateFBO != 0) {
		glDeleteFramebuffers(1, &intermediateFBO);
		glDeleteTextures(1, &screenBuffer);

		intermediateFBO = 0;
		screenBuffer = 0;
	}

	if (edgeFBO != 0) {
		uint32_t smaaFBO[2] = { edgeFBO, blendFBO };
		uint32_t smaaBuffers[2] = { edgeColorBuffer, blendColorBuffer };

		glDeleteFramebuffers(2, smaaFBO);
		glDeleteTextures(2, smaaBuffers);

		edgeFBO = 0; blendFBO = 0;
		edgeColorBuffer = 0; blendColorBuffer = 0;
	}
}

void PostProcessMaster::_init(int width, int height) {
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	uint32_t status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Error initializing post processing framebuffer" << std::endl;
		exit(-1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void PostProcessMaster::_initIntermediateFBO(int width, int height) {
	glGenFramebuffers(1, &intermediateFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

	glGenTextures(1, &screenBuffer);

	glBindTexture(GL_TEXTURE_2D, screenBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenBuffer, 0);

	uint32_t status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Intermediate framebuffer initialization error" << std::endl;
		exit(-1);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessMaster::_initSMAA(int width, int height) {
	glGenFramebuffers(1, &edgeFBO);
	glGenFramebuffers(1, &blendFBO);

	glGenTextures(1, &edgeColorBuffer);
	glBindTexture(GL_TEXTURE_2D, edgeColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, edgeFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, edgeColorBuffer, 0);

	uint32_t status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Error initializing post processing framebuffer" << std::endl;
		exit(-1);
	}

	glGenTextures(1, &blendColorBuffer);
	glBindTexture(GL_TEXTURE_2D, blendColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, blendFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blendColorBuffer, 0);

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Error initializing post processing framebuffer" << std::endl;
		exit(-1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessMaster::calcSMAAbuffers(uint32_t screenTexture, int width, int height) {
	// edge calculation pass
	glBindFramebuffer(GL_FRAMEBUFFER, edgeFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	edgeShader.useShader();

	edgeShader.setInt("screenTexture", 0);
	edgeShader.setVec2("screenResolution", glm::ivec2(screenDimensions.z, screenDimensions.w));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screenTexture);

	quad.renderQuad();

	edgeShader.endShader();

	// blend weight calculation pass
	glBindFramebuffer(GL_FRAMEBUFFER, blendFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	blendShader.useShader();

	blendShader.setInt("edgeTexture", 0);
	blendShader.setVec2("screenResolution", screenDimensions);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, edgeColorBuffer);

	quad.renderQuad();

	blendShader.endShader();
}

void PostProcessMaster::postProcessingPass(bool applyBloom, float knee, float filterRadius, float exposure) {
	if (applyBloom) {
		bloom.setKnee(knee);
		bloom.renderBloomTexture(HDR::Framebuffer().ColorbufferID(), filterRadius, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	shader.useShader();

	shader.setiVec2("screenResolution", glm::ivec2(screenDimensions.z, screenDimensions.w));
	shader.setFloat("exposure", exposure);
	shader.setUint("enableBloom", applyBloom);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, HDR::Framebuffer().ColorbufferID());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bloom.bloomTexture());

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, blendColorBuffer);

	quad.renderQuad();

	glActiveTexture(GL_TEXTURE0);

	shader.endShader();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessMaster::postProcessingPassMSAA(bool applyBloom, float knee, float filterRadius, float exposure) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, HDR::Framebuffer().FramebufferID());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
	glBlitFramebuffer(
		screenDimensions.x, screenDimensions.y, screenDimensions.z, screenDimensions.w,
		screenDimensions.x, screenDimensions.y, screenDimensions.z, screenDimensions.w,
		GL_COLOR_BUFFER_BIT, GL_NEAREST
	);

	if (applyBloom) {
		bloom.setKnee(knee);
		bloom.renderBloomTexture(screenBuffer, filterRadius, FBO);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	shader.useShader();

	shader.setiVec2("screenResolution", screenDimensions);
	shader.setFloat("exposure", exposure);
	shader.setUint("enableBloom", applyBloom);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screenBuffer);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bloom.bloomTexture());

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, blendColorBuffer);

	quad.renderQuad();

	glActiveTexture(GL_TEXTURE0);

	shader.endShader();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}