#include "../../../headers/Engine Core/Fluid Simulation/FluidSimulation.h"

uint32_t fluidSSBO = 0;

void initFluidSimulation(int numParticles) {
	glGenBuffers(1, &fluidSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, fluidSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Particle) * numParticles, NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, fluidSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void renderFluidSimulation() {
	static Shader compute{ "src/shaders/fluid.comp" };
	static Shader render{ "src/shaders/fluid.vert", "src/shaders/fluid.frag" };

	compute.useShader();
	compute.dispatchComputeShader(256, 1, 1, GL_SHADER_STORAGE_BARRIER_BIT);
	compute.endShader();

	render.useShader();

	bindAttriblessVAO();
	glDrawArrays(GL_POINTS, 0, 10000);

	render.endShader();
}