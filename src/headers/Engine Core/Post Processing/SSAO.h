#ifndef _SSAO_H_
#define _SSAO_H_

#include "../../Engine Core/Shader.h"
#include "../../Engine Core/GBuffer.h"
#include "../../Application/AttriblessRendering.h"

class SSAO final : public Framebuffer {
private:
	uint32_t blurFBO{ 0 }, colorBuffer{ 0 }, colorBufferBlur{ 0 }, noiseTexture{ 0 };

	std::vector<glm::vec3> kernel;
	std::vector<glm::vec3> noise;

	glm::vec2 scrResolution{ 0.f };

	float radius{ 50.f }, bias{ 2.f }, occlusionPower{ 10.f };

	Shader shader{ "src/shaders/ssao.vert", "src/shaders/ssao.frag" };
	Shader blurShader{ "src/shaders/ssao.vert", "src/shaders/ssaoBlur.frag" };

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

	void ssaoBlur();
	virtual void clearBuffers() override;

public:
	SSAO(int windowWidth, int windowHeight);

	uint32_t occlusionBuffer() const { return this->colorBufferBlur; }

	SSAO& operator=(const SSAO&) = delete;
	static SSAO& Framebuffer() {
		static SSAO buffer{ 800, 600 };
		return buffer;
	}

	void resize(int width, int height) {
		width = std::max(1, width);
		height = std::max(1, height);

		scrResolution = glm::vec2((float)width, (float)height);

		clearBuffers();
		_init(width, height);
	}
	void setRadius(float radius) { this->radius = radius; }
	void setBias(float bias) { this->bias = bias; }
	void setOcclusionPower(float power) { this->occlusionPower = power; }

	void _init(int windowWidth, int windowHeight);
	void calcSSAO(uint32_t currFramebuffer = 0);

	~SSAO() {
		clearBuffers();
	}
};

#endif