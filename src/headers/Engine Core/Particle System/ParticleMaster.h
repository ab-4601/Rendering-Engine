#ifndef _PARTICLE_MASTER_H_
#define _PARTICLE_MASTER_H_

#include "Particle.h"
#include "ParticleTexture.h"
#include "ParticleRenderer.h"

class ParticleMaster {
private:
	friend CParticle;
	std::vector<CParticle*> particles;
	ParticleRenderer renderer;

	ParticleTexture texture;

public:
	ParticleMaster(ParticleTexture texture = ParticleTexture("src/Content/Textures/Particle System/cosmic.png", 4.f)) {
		renderer.setTexture(texture);
		particles = {};
	}

	ParticleMaster(const ParticleMaster& source) noexcept;

	ParticleMaster(ParticleMaster&& source) noexcept;

	void setParticleTexture(const ParticleTexture& texture) {
		this->texture = texture;
		renderer.setTexture(this->texture);
	}

	void update(float delta, glm::vec3 cameraPosition);

	void renderParticles(const Window* const currWindow, const Camera& camera,
		glm::mat4& model, const glm::mat4& projection) {
		renderer.render(particles, currWindow, camera, model, projection);
	}

	void generateParticle(vec3 position, vec3 velocity, vec3 color, float forceEffect, float rotation,
		float scale, float lifeTime);

	~ParticleMaster() {
		for (auto& particle : this->particles)
			delete particle;
	}
};

#endif