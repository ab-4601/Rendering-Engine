#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "../../headers/Engine Core/Window.h"
#include "../Core Includes/Core.h"

enum Direction : int { FORWARD = 1, BACKWARD = -1, STRAFE_RIGHT = 1, STRAFE_LEFT = -1, UP = 1, DOWN = -1 };

struct CameraData {
	glm::mat4 projection{ 1.f };
	glm::mat4 view{ 1.f };
	glm::vec3 position{ 0.f };
};

class Camera {
private:
	glm::vec3 front, up, right;
	glm::vec3 worldUpDir;

	uint32_t cameraBuffer = 0;

	float pitch, yaw, roll;
	float fov = 45.f, nearPlane = ::near_plane, farPlane = ::far_plane, aspect = 16.f / 9.f;

	float movementSpeed, turnSpeed, movementSpeedMultiplier, velocity;
	CameraData cameraData;

	void update();
	inline void updatePosition(Direction direction, const glm::vec3& directionVector) {
		cameraData.position += (float)direction * directionVector * velocity;
	}

public:
	Camera(glm::vec3 position = glm::vec3(0.f, 0.f, 3.f), int windowWidth = 800, int windowHeight = 600,
		glm::vec3 worldUpDir = glm::vec3(0.f, 1.f, 0.f), float pitch = 0.f, float yaw = 0.f,
		float movementSpeed = 200.f, float turnSpeed = 0.1f);

	void keyFunctionality(const Window* currWindow, float deltaTime);
	void mouseFunctionality(float xChange, float yChange, float scrollChange);

	float getFOV() const { return fov; }
	float getNearPlane() const { return nearPlane; }
	float getFarPlane() const { return farPlane; }

	void setFOV(float fov) {
		this->fov = fov;
		cameraData.projection = glm::perspective(glm::radians(this->fov), aspect, nearPlane, farPlane);
	}

	void setFrustumPlanes(float nearPlane, float farPlane) {
		this->nearPlane = nearPlane;
		this->farPlane = farPlane;

		cameraData.projection = glm::perspective(fov, aspect, this->nearPlane, this->farPlane);
	}

	const glm::mat4& getProjectionMatrix() const { return cameraData.projection; };
	const glm::vec3& getFrontVector() const { return front; }
	const glm::vec3& getRightVector() const { return right; }
	const glm::vec3& getUpVector() const { return up; }
	const glm::vec3& getCameraPosition() const { return cameraData.position; }

	glm::mat4 generateViewMatrix() const { return glm::lookAt(cameraData.position, cameraData.position + front, up); }

	~Camera() = default;
};

#endif
