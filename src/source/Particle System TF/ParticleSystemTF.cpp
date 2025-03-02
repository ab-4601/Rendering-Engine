#include "../../headers/Particle System TF/ParticleSystemTF.h"

Psystem::Psystem(glm::vec3 emitterPosition, int numParticles) {
	this->num_particles = numParticles;
	texture.loadTexture();

	model = glm::translate(model, emitterPosition);
	model = glm::scale(model, glm::vec3(500.f));

	glGenTransformFeedbacks(2, TFO);

	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);

	const int stride = 8 * sizeof(float);

	for (int i = 0; i < 2; i++) {
		glBindVertexArray(VAO[i]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);

		glBufferData(GL_ARRAY_BUFFER, 8 * num_particles * sizeof(float), NULL, GL_DYNAMIC_DRAW);
		float zero = 0.f;
		glClearNamedBufferData(VBO[i], GL_R32F, GL_RED, GL_FLOAT, (void*)&zero);

		glEnableVertexAttribArray(ParticleAttribs::PARTICLE_POSITION);
		glVertexAttribPointer(ParticleAttribs::PARTICLE_POSITION, 3, GL_FLOAT, false, stride, (void*)offsetof(XParticle, position));

		glEnableVertexAttribArray(ParticleAttribs::PARTICLE_VELOCITY);
		glVertexAttribPointer(ParticleAttribs::PARTICLE_VELOCITY, 3, GL_FLOAT, false, stride, (void*)offsetof(XParticle, velocity));

		glEnableVertexAttribArray(ParticleAttribs::PARTICLE_LIFETIME);
		glVertexAttribPointer(ParticleAttribs::PARTICLE_LIFETIME, 1, GL_FLOAT, false, stride, (void*)offsetof(XParticle, lifetime));

		glEnableVertexAttribArray(ParticleAttribs::ELAPSED_TIME);
		glVertexAttribPointer(ParticleAttribs::ELAPSED_TIME, 1, GL_FLOAT, false, stride, (void*)offsetof(XParticle, elapsedTime));

		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, TFO[i]);

		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, posVarying, VBO[i]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, velVarying, VBO[i]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, lifetimeVarying, VBO[i]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, elapsedTimeVarying, VBO[i]);

		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	shader.transformFeedbackRelink(feedbackVaryings, 4, GL_INTERLEAVED_ATTRIBS, "src/shaders/pSystem.vert", "src/shaders/pSystem.frag");
}

void Psystem::render(float time, float particleDelta) {
	shader.useShader();

	shader.setFloat("time", time);
	shader.setFloat("particleDelta", particleDelta);
	shader.setMat4("model", model);
	shader.setInt("particleTexture", 0);
	shader.setInt("numRows", (int)texture.getNumberOfRows());

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(false);

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, TFO[write_index]);

	texture.useTexture();

	glBeginTransformFeedback(GL_POINTS);
	glBindVertexArray(VAO[read_index]);
	glDrawArrays(GL_POINTS, 0, num_particles);
	glEndTransformFeedback();

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

	read_index = 1 - read_index;
	write_index = 1 - write_index;

	glDepthMask(true);
	glDisable(GL_BLEND);
}