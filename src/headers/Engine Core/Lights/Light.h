#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "../../Core Includes/GLM_Core.h"

class Light {
protected:
	glm::vec3 color;

	float ambientIntensity;
	float diffuseIntensity;

public:
	Light(float aIntensity, float dIntensity, glm::vec3 color)
		: color{ color }, ambientIntensity{ aIntensity }, diffuseIntensity{ dIntensity } { }

	virtual glm::vec3 getLightColor() {
		return color;
	}

	glm::vec3 getColor() const { return this->color; }
	float getAmbientIntensity() const { return ambientIntensity; }
	float getDiffuseIntensity() const { return diffuseIntensity; }

	~Light() = default;
};

#endif