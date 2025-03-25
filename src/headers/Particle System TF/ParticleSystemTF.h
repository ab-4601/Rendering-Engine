#ifndef _PARTICLE_SYSTEM_TRANSFORM_FEEDBACK_
#define _PARTICLE_SYSTEM_TRANSFORM_FEEDBACK_

#include "../Core Includes/GLM_Core.h"
#include "../../headers/Engine Core/Shader.h"
#include "../../headers/Engine Core/Particle System/ParticleTexture.h"

enum ParticleAttribs : int{ PARTICLE_POSITION = 0, PARTICLE_VELOCITY = 1, PARTICLE_LIFETIME = 2, ELAPSED_TIME = 3 };

struct XParticle {
	glm::vec3 position = glm::vec3(0.f);
	glm::vec3 velocity = glm::vec3(0.f);
	float lifetime = 0.f;
	float elapsedTime = 0.f;
};

class Psystem {
private:
	static const int posVarying = 0;
	static const int velVarying = 1;
	static const int lifetimeVarying = 2;
	static const int elapsedTimeVarying = 3;

	int num_particles;
	int read_index = 0, write_index = 1;
	
	glm::mat4 model{ 1.f };
	
	const char* feedbackVaryings[4] = { "pos_out", "vel_out", "lifetime_out", "elapsed_time_out" };

	uint32_t TFO[2] = { 0, 0 };
	uint32_t VAO[2] = { 0, 0 };
	uint32_t VBO[2] = { 0, 0 };

	Shader shader{ "src/shaders/pSystem.vert", "src/shaders/pSystem.frag" };
	ParticleTexture texture{ "src/Content/Textures/Particle System/particleAtlas.png", 4.f };

public:
	Psystem(glm::vec3 emitterPosition = glm::vec3(0.f), int numParticles = 100000);

	void render(float time, float particleDelta);

	~Psystem() = default;
};

#endif