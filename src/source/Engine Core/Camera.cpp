#include "../../headers/Engine Core/Camera.h"

Camera::Camera(glm::vec3 position, int windowWidth, int windowHeight, glm::vec3 worldUpDir,
	float pitch, float yaw, float movementSpeed, float turnSpeed) :
	worldUpDir{ worldUpDir }, pitch{ pitch }, yaw{ yaw }, velocity{ 0.f },
	movementSpeed{ movementSpeed }, turnSpeed{ turnSpeed }, movementSpeedMultiplier{ 1.f }
{
	cameraData.position = position;
	velocity = movementSpeed * movementSpeedMultiplier;

	glm::vec3 origin(0.f, 0.f, 0.f);
	front = glm::normalize(cameraData.position - origin);

	float aspect = (float)windowWidth / windowHeight;
	cameraData.projection = glm::perspective(glm::radians(45.f), aspect, ::near_plane, ::far_plane);
	cameraData.view = generateViewMatrix();

	glGenBuffers(1, &cameraBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, cameraBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraData), NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Camera::keyFunctionality(const Window* currWindow, float deltaTime) {
	velocity = movementSpeed * deltaTime * movementSpeedMultiplier;

	if (currWindow->getKeyPress(GLFW_KEY_W) || currWindow->getKeyPress(GLFW_KEY_UP))
		updatePosition(Direction::FORWARD, front);

	if (currWindow->getKeyPress(GLFW_KEY_S) || currWindow->getKeyPress(GLFW_KEY_DOWN))
		updatePosition(Direction::BACKWARD, front);

	if (currWindow->getKeyPress(GLFW_KEY_D) || currWindow->getKeyPress(GLFW_KEY_RIGHT))
		updatePosition(Direction::STRAFE_RIGHT, right);

	if (currWindow->getKeyPress(GLFW_KEY_A) || currWindow->getKeyPress(GLFW_KEY_LEFT))
		updatePosition(Direction::STRAFE_LEFT, right);

	if (currWindow->getKeyPress(GLFW_KEY_SPACE))
		updatePosition(Direction::UP, up);

	if (currWindow->getKeyPress(GLFW_KEY_LEFT_CONTROL))
		updatePosition(Direction::DOWN, up);

	if (currWindow->getKeyPress(GLFW_KEY_LEFT_SHIFT))
		movementSpeedMultiplier = 3.f;
	else
		movementSpeedMultiplier = 1.f;
}

void Camera::mouseFunctionality(float xChange, float yChange, float scrollChange) {
	if (movementSpeed > 0)
		movementSpeed += scrollChange * 100;

	if (movementSpeed <= 0)
		movementSpeed = 1.f;

	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw += xChange;
	pitch += yChange;

	if (pitch > 89.f) {
		pitch = 89.f;
	}

	if (pitch < -89.f) {
		pitch = -89.f;
	}

	update();
}

void Camera::update() {
	front.x = cosf(glm::radians(yaw)) * cosf(glm::radians(pitch));
	front.y = sinf(glm::radians(pitch));
	front.z = sinf(glm::radians(yaw)) * cosf(glm::radians(pitch));

	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUpDir));
	up = glm::normalize(glm::cross(right, front));

	cameraData.view = generateViewMatrix();

	glBindBuffer(GL_UNIFORM_BUFFER, cameraBuffer);
	void* memory = glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(CameraData), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
	if (memory) {
		memcpy(memory, &cameraData, sizeof(CameraData));
		glUnmapBuffer(GL_UNIFORM_BUFFER);
	}
	else
		std::cerr << "Failed to map uniform camera data buffer" << std::endl;
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}