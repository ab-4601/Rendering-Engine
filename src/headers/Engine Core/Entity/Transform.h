#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "../../Core Includes/GLM_Core.h"

struct Plane {
	glm::vec3 normal;
	float distance;

	Plane(glm::vec3 point = glm::vec3{ 0.f }, glm::vec3 norm = glm::vec3{ 0.f, 1.f, 0.f })
		: normal{ glm::normalize(norm) }, distance{ glm::dot(point, normal) } {
	}

	float getSignedDistanceToPlane(const glm::vec3& point) const {
		return glm::dot(normal, point) - distance;
	}
};

struct Frustum {
	Plane topPlane;
	Plane bottomPlane;

	Plane rightPlane;
	Plane leftPlane;

	Plane nearPlane;
	Plane farPlane;
};

Frustum createFrustumFromCamera(const Camera& camera, float aspect, float fovY, float zNear, float zFar) {
	Frustum frustum{};
	const float halfVSide = zFar * tanf(fovY * 0.5f);
	const float halfHSide = halfVSide * aspect;

	const glm::vec3 frontMultFar = zFar * camera.getFrontVector();
	const glm::vec3 cameraFront = camera.getFrontVector();
	const glm::vec3 cameraPosition = camera.getCameraPosition();
	const glm::vec3 cameraRight = camera.getRightVector();
	const glm::vec3 cameraUp = camera.getUpVector();

	frustum.nearPlane = { cameraPosition + zNear * cameraFront, cameraFront };
	frustum.farPlane = { cameraPosition + frontMultFar, -cameraFront };
	frustum.rightPlane = { cameraPosition, glm::cross(frontMultFar - cameraRight * halfHSide, cameraUp) };
	frustum.leftPlane = { cameraPosition, glm::cross(cameraUp, frontMultFar + cameraRight * halfHSide) };
	frustum.topPlane = { cameraPosition, glm::cross(cameraRight, frontMultFar - cameraUp * halfVSide) };
	frustum.bottomPlane = { cameraPosition, glm::cross(frontMultFar + cameraUp * halfVSide, cameraRight) };

	return frustum;
}

class Transform {
protected:
	glm::vec4 position{ 0.f };
	glm::vec4 rotation{ 0.f };
	glm::vec3 scale{ 1.f };
	glm::mat4 model{ 1.f };

	bool m_isDirty = true;

	glm::mat4 getLocalModelMatrix() const {
		const glm::mat4 transformX = glm::rotate(glm::mat4(1.f), glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
		const glm::mat4 transformY = glm::rotate(glm::mat4(1.f), glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f));
		const glm::mat4 transformZ = glm::rotate(glm::mat4(1.f), glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));

		const glm::mat4 rotationMatrix = transformX * transformY * transformZ;

		return glm::translate(glm::mat4(1.f), glm::vec3(position)) * rotationMatrix * glm::scale(glm::mat4(1.f), scale);
	}

public:
	void computeModelMatrix(const glm::mat4& parentModelMatrix = glm::mat4(1.f)) {
		model = parentModelMatrix * getLocalModelMatrix();
		m_isDirty = false;
	}

	void setLocalPosition(const glm::vec3& position) {
		this->position = glm::vec4(position, 1.f);
		m_isDirty = false;
	}

	void setLocalRotation(const glm::vec3& rotation) {
		this->rotation = glm::vec4(rotation, 1.f);
		m_isDirty = true;
	}

	void setLocalScale(const glm::vec3 scale) {
		this->scale = scale;
		m_isDirty = true;
	}

	const glm::vec3& getGlobalPosition() const { return model[3]; }
	const glm::vec4& getLocalPosition() const { return position; }
	const glm::vec4& getLocalRotation() const { return rotation; }
	const glm::vec3& getLocalScale() const { return scale; }
	const glm::mat4& getModelMatrix() const { return model; }
	const glm::vec3& getRightVector() const { return model[0]; }
	const glm::vec3& getUpVector() const { return model[1]; }
	const glm::vec3& getBackwardVector() const { return model[2]; }
	const glm::vec3& getForwardVector() const { return -model[3]; }

	const glm::vec3 getGlobalScale() const {
		float x = glm::length(getRightVector());
		float y = glm::length(getUpVector());
		float z = glm::length(getBackwardVector());

		return glm::vec3(x, y, z);
	}

	bool isDirty() const { return m_isDirty; }
};

#endif