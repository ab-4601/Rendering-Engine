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

	CoordinateSystem& operator=(const CoordinateSystem&) = delete;
	static CoordinateSystem& Widget() {
		static CoordinateSystem widget;
		return widget;
	}

	void createCoordinateSystem();
	void drawCoordinateSystem(glm::ivec4 viewportDimensions, const Camera& camera);

	~CoordinateSystem();
};

#endif