#ifndef _GBUFFER_H_
#define _GBUFFER_H_

#include "Mesh/Model.h"

class GBuffer {
private:
	uint32_t FBO{ 0 }, RBO{ 0 }, gPosition{ 0 }, gNormal{ 0 }, gAlbedo{ 0 }, gMetallic{ 0 };

	bool drawWireframe{ false };
	glm::mat4 viewportMatrix{ 1.f };

	Shader shader{ "src/shaders/gbuffer.vert", "src/shaders/gbuffer.frag", "src/shaders/gbuffer.geom" };

	GLenum colorAttachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

	void genTexture(uint32_t& texID, GLenum colorAttachment, int windowWidth, int windowHeight);
	void _init(int windowWidth, int windowHeight);

	void clearBuffers();

public:
	GBuffer(int windowWidth, int windowHeight);

	uint32_t framebufferID() const { return FBO; }
	uint32_t positionBuffer() const { return gPosition; }
	uint32_t normalBuffer() const { return gNormal; }
	uint32_t albedoBuffer() const { return gAlbedo; }
	uint32_t metallicBuffer() const { return gMetallic; }

	void resize(int width, int height) {
		clearBuffers();
		_init(width, height);
	}

	void updateWireframeBool(bool drawWireframe) { this->drawWireframe = drawWireframe; }
	void updateBuffer(Shader& outlineShader, int meshID, const std::vector<Mesh*>& meshes,
		const std::vector<Model*>& models, uint32_t currFramebuffer = 0);

	~GBuffer() {
		clearBuffers();
	}
};

#endif