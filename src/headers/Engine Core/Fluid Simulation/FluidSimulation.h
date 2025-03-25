#ifndef _FLUID_SIMULATION_H_
#define _FLUID_SIMULATION_H_

#include "../Shader.h"
#include "../../Application/AttriblessRendering.h"

struct Particle {
	glm::vec4 position;
	glm::vec4 velocity;
	glm::vec4 acceleration;
	float density;
	float pressure;
};

extern uint32_t fluidSSBO;

void initFluidSimulation(int numParticles = 10000);
void renderFluidSimulation();

#endif