#include "../../headers/Engine Core/GBuffer.h"

GBuffer::GBuffer(int windowWidth, int windowHeight) {
	glGenFramebuffers(1, &FBO);
	glGenRenderbuffers(1, &RBO);

	glGenTextures(1, &gPosition);
	glGenTextures(1, &gNormal);
	glGenTextures(1, &gAlbedo);
	glGenTextures(1, &gMetallic);

	_init(windowWidth, windowHeight);
}

void GBuffer::genTexture(uint32_t& texID, GLenum colorAttachment, int windowWidth, int windowHeight) {
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, colorAttachment, GL_TEXTURE_2D, texID, 0);
}

void GBuffer::clearBuffers() {
	if (FBO != 0) {
		glDeleteFramebuffers(1, &FBO);

		uint32_t textures[] = { gPosition, gNormal, gAlbedo, gMetallic };
		glDeleteTextures(4, textures);

		FBO = 0; gPosition = 0; gNormal = 0; gAlbedo = 0; gMetallic = 0;
	}
}

void GBuffer::_init(int windowWidth, int windowHeight) {
	viewportMatrix = {
		glm::vec4((float)windowWidth / 2.f, 0.f, 0.f, (float)windowWidth / 2.f),
		glm::vec4(0.f, (float)windowHeight / 2.f, 0.f, (float)windowHeight / 2.f),
		glm::vec4(0.f, 0.f, 1.f, 0.f),
		glm::vec4(0.f, 0.f, 0.f, 1.f)
	};

	shader.useShader();
	shader.setMat4("viewportMatrix", viewportMatrix);
	shader.endShader();

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	genTexture(gPosition, GL_COLOR_ATTACHMENT0, windowWidth, windowHeight);
	genTexture(gNormal, GL_COLOR_ATTACHMENT1, windowWidth, windowHeight);
	genTexture(gAlbedo, GL_COLOR_ATTACHMENT2, windowWidth, windowHeight);
	genTexture(gMetallic, GL_COLOR_ATTACHMENT3, windowWidth, windowHeight);

	glDrawBuffers(4, colorAttachments);

	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Error initializing G-Buffer Framebuffer" << std::endl;
		exit(-1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
}

void GBuffer::updateBuffer(Shader& outlineShader, glm::ivec2 meshDrawData, uint32_t currFramebuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glDrawBuffers(4, colorAttachments);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	shader.useShader();
	shader.setUint("drawWireframe", drawWireframe);

	if (meshDrawData.x != -1) {
		if ((bool)meshDrawData.y == false && meshDrawData.x < (int)Mesh::meshList.size())
			Mesh::meshList[meshDrawData.x]->renderMeshWithOutline(shader, outlineShader);

		else if ((bool)meshDrawData.y == true && meshDrawData.x < (int)Model::modelList.size())
			Model::modelList[meshDrawData.x]->renderModelWithOutline(shader, outlineShader);
	}

	for (size_t i = 0; i < Mesh::meshList.size(); i++) {
		if ((int)i == meshDrawData.x && meshDrawData.y == false)
			continue;

		Mesh::meshList[i]->renderMesh(shader, GL_TRIANGLES);
	}

	for (size_t i = 0; i < Model::modelList.size(); i++) {
		if ((int)i == meshDrawData.x && (bool)meshDrawData.y == true)
			continue;

		Model::modelList[i]->renderModel(shader, GL_TRIANGLES);
	}

	shader.endShader();
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glBindFramebuffer(GL_FRAMEBUFFER, currFramebuffer);
}