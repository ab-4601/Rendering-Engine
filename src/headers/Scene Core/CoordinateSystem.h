#ifndef _COORDINATE_SYSTEM_H_
#define _COORDINATE_SYSTEM_H_

#include "../Engine Core/Mesh/Geometry/Line.h"
#include "../Engine Core/Shader.h"
#include "../Engine Core/Camera.h"

class CoordinateSystem {
private:
	std::vector<Line*> axes;

	Shader shader{ "src/shaders/coord.vert", "src/shaders/coord.frag" };
	glm::mat4 model = glm::mat4(1.f);

public:
	CoordinateSystem();

	void createCoordinateSystem();
	void drawCoordinateSystem(int bufferWidth, int bufferHeight, const Camera& camera);

	~CoordinateSystem();
};

#endif