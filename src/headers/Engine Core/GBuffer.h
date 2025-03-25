#ifndef _GBUFFER_H_
#define _GBUFFER_H_

#include "Mesh/Model.h"
#include "Framebuffer.h"

class GBuffer final : public Framebuffer {
private:
	uint32_t RBO{ 0 }, gPosition{ 0 }, gNormal{ 0 }, gAlbedo{ 0 }, gMetallic{ 0 };

	bool drawWireframe{ false };
	glm::mat4 viewportMatrix{ 1.f };

	Shader shader{ "src/shaders/gbuffer.vert", "src/shaders/gbuffer.frag", "src/shaders/gbuffer.geom" };

	GLenum colorAttachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

	void setColorAttachment(uint32_t& texID, GLenum colorAttachment, int windowWidth, int windowHeight);
	void _init(int windowWidth, int windowHeight);

	virtual void clearBuffers() override;

public:
	GBuffer(int windowWidth, int windowHeight);

	uint32_t framebufferID() const { return FBO; }
	uint32_t positionBuffer() const { return gPosition; }
	uint32_t normalBuffer() const { return gNormal; }
	uint32_t albedoBuffer() const { return gAlbedo; }
	uint32_t metallicBuffer() const { return gMetallic; }

	GBuffer& operator=(const GBuffer&) = delete;
	static GBuffer& Framebuffer() {
		static GBuffer buffer{ 800, 600 };
		return buffer;
	}

	void resize(int width, int height) {
		width = std::max(1, width);
		height = std::max(1, height);

		_init(width, height);
	}

	void updateWireframeBool(bool drawWireframe) { this->drawWireframe = drawWireframe; }
	void updateBuffer(Shader& outlineShader, glm::ivec2 meshDrawData, uint32_t currFramebuffer = 0);

	~GBuffer() {
		clearBuffers();
	}
};

#endif