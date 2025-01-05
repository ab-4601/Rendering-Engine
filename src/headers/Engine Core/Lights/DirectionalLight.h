#ifndef _DIRECTIONAL_LIGHT_H_
#define _DIRECTIONAL_LIGHT_H_

#include "Light.h"

class DirectionalLight : public Light {
private:
	glm::vec3 direction;

public:
	DirectionalLight(float aIntensity, float dIntensity, glm::vec3 direction, glm::vec3 color)
		: Light(aIntensity, dIntensity, color), direction{ direction } { }

	inline void updateLightLocation(glm::vec3 pos) {
		this->direction = pos;
	}

	glm::vec3 getLightDirection() const { return this->direction; }

	~DirectionalLight() = default;
};

#endif