#ifndef _GRID_H_
#define _GRID_H_

#include "../Engine Core/Camera.h"
#include "../Engine Core/Shader.h"

class Grid {
private:
	float vertices[12]{
		-1.f,  0.f, -1.f,
		 1.f,  0.f, -1.f,
		 1.f,  0.f,  1.f,
		-1.f,  0.f,  1.f
	};

	uint32_t indices[6]{
		0, 2, 1,
		2, 0, 3
	};

	uint32_t VAO, VBO, IBO;
	Shader shader{ "src/shaders/grid.vert", "src/shaders/grid.frag" };

public:
	Grid();

	void renderGrid();

	~Grid();
};

#endif