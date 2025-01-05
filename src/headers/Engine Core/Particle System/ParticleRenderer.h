#ifndef _PARTICLE_RENDERER_H_
#define _PARTICLE_RENDERER_H_

#include "../Shader.h"
#include "../Window.h"
#include "../Camera.h"
#include "Particle.h"
#include "ParticleTexture.h"

class ParticleRenderer {
private:
	float vertices[12]{
		-0.5f,  0.5f,  0.f,
		 0.5f,  0.5f,  0.f,
		-0.5f, -0.5f,  0.f,
		 0.5f, -0.5f,  0.f
	};

	uint32_t indices[6]{
		0, 1, 2,
		1, 2, 3
	};

	static const int maxInstances = 10000;
	static const int instanceDataLength = 21;

	float* dataBuffer = new float[maxInstances * instanceDataLength];
	int pointer;

	uint32_t VAO, VBO, IBO;
	uint32_t iVBO;

	Shader shader{ "src/shaders/particle.vert", "src/shaders/particle.frag", "src/shaders/particle.geom" };
	ParticleTexture texture;

	glm::vec2 texOffset1;
	glm::vec2 texOffset2;
	float blend;

	void updateTextureCoordInfo(const CParticle* const particle);
	void setTextureOffset(glm::vec2& offset, int index);
	void updateModelViewMatrix(mat4& model, mat4& view, vec3 position, float rotation, float scale);

	void createEmptyVBO(int floatCount);
	void addInstancedAttribute(int attribute, int dataSize, int instancedDataLength, int offset);
	void updateVBO();

public:
	ParticleRenderer(ParticleTexture texture = ParticleTexture("src/Content/Textures/Particle System/cosmic.png"));

	void render(const std::vector<CParticle*>& particles, const Window* const currWindow,
		const Camera& camera, glm::mat4& modelViewMatrix, const glm::mat4& projection);

	float getBlend() const { return blend; }

	void setTexture(ParticleTexture& particleTexture) {
		texture = particleTexture;
		texture.loadTexture();
	}

	void drawParticleSprites(size_t primitiveCount) const {
		glFrontFace(GL_CW);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

		glDrawElementsInstanced(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, NULL, static_cast<GLsizei>(primitiveCount));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	~ParticleRenderer();
};

#endif