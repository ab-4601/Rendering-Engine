#ifndef _POINT_LIGHT_H_
#define _POINT_LIGHT_H_

#include "Light.h"

class PointLight : public Light {
protected:
	glm::vec3 position;
	float constant, linear, exponent;

public:
	PointLight(float aIntensity, float dIntensity,
		glm::vec3 position, float constant, float linear, float exponent, glm::vec3 color)
		: Light(aIntensity, dIntensity, color), position{ position }, 
		constant{ constant }, linear{ linear }, exponent{ exponent } { }

	PointLight(PointLight&& source) noexcept
		: Light(source.ambientIntensity, source.diffuseIntensity, source.color), position{ source.position },
		constant{ source.constant }, linear{ source.linear }, exponent{ source.exponent } 
	{
		source.~PointLight();
	}

	PointLight(const PointLight& source) noexcept
		: Light(source.ambientIntensity, source.diffuseIntensity, source.color), position{ source.position },
		constant{ source.constant }, linear{ source.linear }, exponent{ source.exponent } { }

	glm::vec3 getPosition() const { return position; }
	float getAttenuationConstant() const { return constant; }
	float getAttenuationLinear() const { return linear; }
	float getAttenuationExponent() const { return exponent; }

	virtual void updateLightLocation(glm::vec3 pos) {
		this->position = pos;
	}

	void operator=(PointLight&& rhs) noexcept {
		ambientIntensity = rhs.ambientIntensity;
		diffuseIntensity = rhs.diffuseIntensity;
		color = rhs.color;
		position = rhs.position;
		constant = rhs.constant;
		linear = rhs.linear;
		exponent = rhs.exponent;
	}

	~PointLight() = default;
};

#endif