#ifndef _SSAO_H_
#define _SSAO_H_

#include "../../Engine Core/Shader.h"
#include "../../Engine Core/Mesh/Geometry/Quad.h"

class SSAO {
private:
	uint32_t FBO{ 0 }, blurFBO{ 0 }, colorBuffer{ 0 }, colorBufferBlur{ 0 }, noiseTexture{ 0 };

	std::vector<glm::vec3> kernel;
	std::vector<glm::vec3> noise;

	glm::vec2 scrResolution{ 0.f };

	float radius{ 50.f }, bias{ 2.f }, occlusionPower{ 10.f };

	Shader shader{ "src/shaders/ssao.vert", "src/shaders/ssao.frag" };
	Shader blurShader{ "src/shaders/ssao.vert", "src/shaders/ssaoBlur.frag" };

	Quad quad;

	float jitter(float x, float y) {
		static std::default_random_engine generator;
		static std::uniform_real_distribution<float> randomFloat(x, y);

		return randomFloat(generator);
	}

	void genTexture(uint32_t& texID, GLenum colorAttachment, GLint internalFormat, GLenum format,
		int windowWidth, int windowHeight);

	void genNoiseTexture(int windowWidth, int windowHeight);

	float lerp(float a, float b, float c) {
		return a + c * (b - a);
	}

	bool checkFBOStatus() {
		uint32_t status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (status != GL_FRAMEBUFFER_COMPLETE)
			return false;

		return true;
	}

	void ssaoBlur();

public:
	SSAO(int windowWidth, int windowHeight);

	uint32_t occlusionBuffer() const { return this->colorBufferBlur; }

	void setRadius(float radius) { this->radius = radius; }
	void setBias(float bias) { this->bias = bias; }
	void setOcclusionPower(float power) { this->occlusionPower = power; }

	void _init(int windowWidth, int windowHeight);
	void calcSSAO(uint32_t gPosition, uint32_t gNormal, uint32_t currFramebuffer = 0);

	~SSAO() {
		if (this->FBO != 0)
			glDeleteFramebuffers(1, &this->FBO);
	}
};

#endif