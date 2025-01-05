#ifndef _LINE_H_
#define _LINE_H_

#pragma once

#include <vector>
#include "../../../../headers/Core Includes/GL_Core.h"
#include "../../../../headers/Core Includes/GLM_Core.h"

class Line {
private:
	uint32_t VAO, VBO, IBO;

	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	glm::vec3 a;
	glm::vec3 b;

	glm::vec3 color;

public:
	Line(glm::vec3 a = { 0.f, 0.f, 0.f }, glm::vec3 b = { 0.f, 0.f, 0.f }, glm::vec3 color = { 0.f, 0.f, 0.f });

	void createMesh();
	void renderMesh(GLenum renderMode = GL_LINES);
	void clearMesh();

	inline glm::vec3 getColor() const { return this->color; }

	~Line() {
		this->clearMesh();
	}
};

#endif