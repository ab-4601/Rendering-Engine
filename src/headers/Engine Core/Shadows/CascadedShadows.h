#ifndef _CASCADED_SHADOWS_H_
#define _CASCADED_SHADOWS_H_

#include <iostream>
#include <random>
#include "../../Core Includes/Core.h"
#include "../../Engine Core/Mesh/Model.h"

class CascadedShadows {
private:
	static const int mapResolution = 2048;
	static const uint32_t numCascades = 4;

	uint32_t FBO{ 0 }, SSBO{ 0 };
	uint32_t shadowMaps{ 0 }, randomOffset{ 0 };

	float cascadeSplits[::MAX_CASCADES];
	glm::mat4 lightSpaceMatrices[::MAX_CASCADES];
	glm::vec4 frustumCorners[8];
	glm::vec3 noiseTextureSize{ 0.f };

	float aspect{ 0.f };

	Shader shader{ "src/shaders/CSM.vert", "src/shaders/CSM.frag", "src/shaders/CSM.geom" };
	Shader computeShader{ "src/shaders/CSM.comp" };

	float jitter(float x, float y) {
		static std::default_random_engine generator;
		static std::uniform_real_distribution<float> randomFloat(x, y);

		return randomFloat(generator);
	}

	bool checkFramebufferStatus();
	void calcSplitDepths(float lambda);
	void genRandomOffsetData(int size, int samplesU, int samplesV);
	void setComputeUniforms();

public:
	CascadedShadows(int windowWidth = 800, int windowHeight = 600, float lambda = 0.5f,
		int size = 10, int samplesU = 4, int samplesV = 4);

	void _init();

	uint32_t getNumCascades() const { return this->numCascades; }
	uint32_t getShadowMaps() const { return this->shadowMaps; }
	uint32_t noiseBuffer() const { return this->randomOffset; }
	const float* const cascadePlanes() const { return this->cascadeSplits; }
	glm::vec3 getNoiseTextureSize() const { return this->noiseTextureSize; }

	void calculateShadows(int windowWidth, int windowHeight, const std::vector<Mesh*>& meshes,
		const std::vector<Model*>& models, const glm::vec3& lightPosition, uint32_t currFramebuffer = 0);

	~CascadedShadows();
};

#endif