#include "../../headers/Engine Core/Camera.h"

void Camera::init(const Window& window, glm::vec3 position, float movementSpeed, float turnSpeed) {
	cameraData.position = position;
	this->turnSpeed = turnSpeed;
	this->movementSpeed = movementSpeed;
	velocity = movementSpeed * movementSpeedMultiplier;

	glm::vec3 origin(0.f, 0.f, 0.f);
	front = glm::normalize(cameraData.position - origin);
	
	aspect = (float)window.getBufferWidth() / window.getBufferHeight();
	nearPlane = ::near_plane; farPlane = ::far_plane;
	fov = 45.f;

	cameraData.projection = glm::perspective(glm::radians(45.f), aspect, nearPlane, farPlane);
	cameraData.view = generateViewMatrix();

	glGenBuffers(1, &cameraBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, cameraBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraData), NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Camera::keyFunctionality(const Window& currWindow) {
	velocity = movementSpeed * deltaTime * movementSpeedMultiplier;

	if (currWindow.getKeyPress(GLFW_KEY_W) || currWindow.getKeyPress(GLFW_KEY_UP))
		updatePosition(Direction::FORWARD, front);

	if (currWindow.getKeyPress(GLFW_KEY_S) || currWindow.getKeyPress(GLFW_KEY_DOWN))
		updatePosition(Direction::BACKWARD, front);

	if (currWindow.getKeyPress(GLFW_KEY_D) || currWindow.getKeyPress(GLFW_KEY_RIGHT))
		updatePosition(Direction::STRAFE_RIGHT, right);

	if (currWindow.getKeyPress(GLFW_KEY_A) || currWindow.getKeyPress(GLFW_KEY_LEFT))
		updatePosition(Direction::STRAFE_LEFT, right);

	if (currWindow.getKeyPress(GLFW_KEY_SPACE))
		updatePosition(Direction::UP, up);

	if (currWindow.getKeyPress(GLFW_KEY_LEFT_CONTROL))
		updatePosition(Direction::DOWN, up);

	if (currWindow.getKeyPress(GLFW_KEY_LEFT_SHIFT))
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
}

void Camera::update(Window& window) {
	if(window.getRMBPressed()) {
		keyFunctionality(window);
		mouseFunctionality(window.getXChange(), window.getYChange(), window.getScrollChange());
	}

	front.x = cosf(glm::radians(yaw)) * cosf(glm::radians(pitch));
	front.y = sinf(glm::radians(pitch));
	front.z = sinf(glm::radians(yaw)) * cosf(glm::radians(pitch));

	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUpDir));
	up = glm::normalize(glm::cross(right, front));

	cameraData.view = generateViewMatrix();

	glBindBuffer(GL_UNIFORM_BUFFER, cameraBuffer);

	void* memory = glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(CameraData), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);

	if (memory)
		memcpy(memory, &cameraData, sizeof(CameraData));
	else
		std::cerr << "Failed to map uniform camera data buffer" << std::endl;

	glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}