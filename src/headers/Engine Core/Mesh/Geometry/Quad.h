#ifndef _QUAD_H_
#define _QUAD_H_

#include "../../../Core Includes/GL_Core.h"

class Quad {
private:
	float vertices[20] = {
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};

	uint32_t VAO{ 0 }, VBO{ 0 };

public:
	Quad() = default;

	void createQuad();
	void renderQuad() const;

	~Quad() {
		if (this->VAO != 0)
			glDeleteVertexArrays(1, &VAO);

		if (this->VBO != 0)
			glDeleteBuffers(1, &VBO);
	}
};

#endif