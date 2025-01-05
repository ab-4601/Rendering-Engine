#ifndef _UV_SPHERE_H_
#define _UV_SPHERE_H_

#include "../Mesh.h"

class UVSphere : public Mesh {
private:
	float radius;

	void addVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texel);
	void addIndices(uint32_t a, uint32_t b, uint32_t c);

	void generateIndices(uint32_t stackCount, uint32_t sectorCount);
	void generateSphere(uint32_t stackCount, uint32_t sectorCount);

public:
	UVSphere(uint32_t stackCount = 10, uint32_t sectorCount = 10, float radius = 1.f, GLenum windingOrder = GL_CCW);

	~UVSphere() = default;
};



#endif