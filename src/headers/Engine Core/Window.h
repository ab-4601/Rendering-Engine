#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <iostream>
#include "../Core Includes/GL_Core.h"
#include "../Core Includes/GLM_Core.h"
#include "../Core Includes/Core.h"

enum CONTEXT_VERSION : int { MAJOR = 4, MINOR = 6 };

class Window {
private:
	GLFWwindow* window;
	int windowWidth, windowHeight;
	int bufferWidth, bufferHeight;

	double viewportX, viewportY;
	GLfloat lastX, lastY, XChange, YChange;
	bool mouseFirstMoved;

	GLfloat scrollChange;

	bool keys[1024];
	bool LMBPressed;
	bool RMBPressed;
	bool isWindowFocused;

	static void handleInput(GLFWwindow* window, int key, int code, int action, int mode);
	static void setMousePosition(GLFWwindow* window, double xPos, double yPos);
	static void setMouseScroll(GLFWwindow* window, double xOffset, double yOffset);
	static void getMouseButton(GLFWwindow* window, int button, int action, int mods);
	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
	static void windowFocusCallback(GLFWwindow* window, int focused);
	static void windowRefreshCallback(GLFWwindow* window);

	void createCallbacks() {
		glfwSetKeyCallback(window, handleInput);
		glfwSetCursorPosCallback(window, setMousePosition);
		glfwSetScrollCallback(window, setMouseScroll);
		glfwSetMouseButtonCallback(window, getMouseButton);
		glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
		glfwSetWindowFocusCallback(window, windowFocusCallback);
		glfwSetWindowRefreshCallback(window, windowRefreshCallback);
	}

public:
	Window(int windowWidth = 0, int windowHeight = 0);

	// Setup window
	void setupWindow();
	GLFWwindow* getMainWindow() { return window; }

	// Get buffer dimensions
	int getBufferWidth() const { return bufferWidth; }
	int getBufferHeight() const { return bufferHeight; }

	int getWindowWidth() const { return windowWidth; }
	int getWindowHeight() const { return windowHeight; }

	bool getKeyPress(unsigned int keyVal) const { return keys[keyVal]; }

	bool windowFocused() const { return isWindowFocused; }

	GLFWwindow* getGlfwWindow() const { return window; }
	const Window* getCurrWindow() const { return this; }

	void setLMBPressed(bool val) { LMBPressed = val; }
	void setRMBPressed(bool val) { RMBPressed = val; }

	bool getLMBPressed() const { return LMBPressed; }
	bool getRMBPressed() const { return RMBPressed; }

	glm::vec2 getViewportCoord() const { return glm::vec2(viewportX, viewportY); }

	float getXChange() {
		float delta = XChange;
		XChange = 0.f;
		return delta;
	}

	float getYChange() {
		float delta = YChange;
		YChange = 0.f;
		return delta;
	}

	float getScrollChange() {
		float delta = scrollChange;
		scrollChange = 0.f;
		return delta;
	}

	~Window() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}
};

#endif
