#ifndef _ICOSPHERE_H_
#define _ICOSPHERE_H_

#include "../Mesh.h"

class Icosphere : public Mesh {
private:
	std::vector<uint32_t> tempIndices;

	void addTempIndices(uint32_t a, uint32_t b, uint32_t c) {
		tempIndices.push_back(a);
		tempIndices.push_back(b);
		tempIndices.push_back(c);
	}

	void _generateIcosahedron();
	void _subdivide(glm::vec3 a, glm::vec3 b, glm::vec3 c, uint32_t index1, uint32_t index2, uint32_t index3);
	void _generateTexCoords();
	void _smoothSphere(int subdivisions);

public:
	Icosphere(GLenum windingOrder = GL_CW, int subdivisions = 3);

	~Icosphere() = default;
};

#endif