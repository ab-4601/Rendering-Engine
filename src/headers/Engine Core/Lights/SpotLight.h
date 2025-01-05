#ifndef _SPOT_LIGHT_H_
#define _SPOT_LIGHT_H_

#include "PointLight.h"

class SpotLight : public PointLight
{
private:
	glm::vec3 direction;
	float edge, procEdge;

public:
	SpotLight(float aIntensity, float dIntensity, glm::vec3 position,
		float constant, float linear, float exponent,
		glm::vec3 color, glm::vec3 direction, float edge)
		: PointLight(aIntensity, dIntensity, position, constant, linear, exponent, color),
		direction{ direction }, edge{ edge }, procEdge{ cosf(glm::radians(edge)) } { }

	SpotLight(SpotLight&& source) noexcept
		: PointLight(source.ambientIntensity, source.diffuseIntensity, source.position, source.constant,
			source.linear, source.exponent, source.color), direction{ source.direction }, edge{ source.edge },
		procEdge{ source.procEdge } 
	{
		source.~SpotLight();
	}

	SpotLight(const SpotLight& source) noexcept
		: PointLight(source.ambientIntensity, source.diffuseIntensity, source.position, source.constant,
			source.linear, source.exponent, source.color), direction{ source.direction }, edge{ source.edge },
		procEdge{ source.procEdge } { }

	glm::vec3 getDirection() const { return this->direction; }
	float getEdge() const { return this->edge; }
	float getProcEdge() const { return this->procEdge; }

	void operator=(SpotLight&& rhs) noexcept {
		ambientIntensity = rhs.ambientIntensity;
		diffuseIntensity = rhs.diffuseIntensity;
		color = rhs.color;
		position = rhs.position;
		direction = rhs.direction;
		constant = rhs.constant;
		linear = rhs.linear;
		exponent = rhs.exponent;
		edge = rhs.edge;
		procEdge = rhs.procEdge;
	};

	~SpotLight() = default;
};

#endif