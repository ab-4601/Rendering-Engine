#ifndef _POINT_SHADOWS_H_
#define _POINT_SHADOWS_H_

#pragma once

#include <string>
#include "../../Core Includes/Core.h"
#include "../../Engine Core/Mesh/Mesh.h"

class PointShadow {
private:
	static const int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	uint32_t FBO = 0;
	uint32_t depthMap = 0;

	float nearPlane = 1.f, farPlane = 25.f;

	glm::mat4 projection{ 1.f };
	std::vector<glm::mat4> shadowTransforms{};

	Shader shader{ "src/shaders/shadow.vert", "src/shaders/shadow.frag", "src/shaders/shadow.geom" };

public:
	PointShadow(float nearPlane = 1.f, float farPlane = 25.f);

	void _init();
	void calculateShadowTransforms(glm::vec3 lightPosition);
	void calculateShadowMap(std::vector<Mesh*>& meshes, int windowWidth, int windowHeight,
		glm::vec3 lightPosition, uint32_t currentFramebuffer = 0);

	uint32_t shadowMap() const { return this->depthMap; }

	~PointShadow();
};

#endif