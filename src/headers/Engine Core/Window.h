#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <iostream>
#include "../Core Includes/GL_Core.h"
#include "../Core Includes/GLM_Core.h"
#include "../Core Includes/Core.h"

enum CONTEXT_VERSION : int { MAJOR = 4, MINOR = 6 };

class Window {
private:
	// Window variables
	GLFWwindow* window;
	uint32_t windowWidth, windowHeight;
	int bufferWidth, bufferHeight;

	// Camera variables
	double viewportX, viewportY;
	GLfloat lastX, lastY, XChange, YChange;
	bool mouseFirstMoved;

	GLfloat scrollChange;

	bool keys[1024];
	bool LMBPressed;
	bool RMBPressed;

	// Camera movement and setup
	static void handleInput(GLFWwindow* window, int key, int code, int action, int mode);
	static void setMousePosition(GLFWwindow* window, double xPos, double yPos);
	static void setMouseScroll(GLFWwindow* window, double xOffset, double yOffset);
	static void getMouseButton(GLFWwindow* window, int button, int action, int mods);

	inline void createCallbacks() {
		glfwSetKeyCallback(window, handleInput);
		glfwSetCursorPosCallback(window, setMousePosition);
		glfwSetScrollCallback(window, setMouseScroll);
		glfwSetMouseButtonCallback(window, getMouseButton);
	}

public:
	Window(uint32_t windowWidth = 0, uint32_t windowHeight = 0);

	// Setup window
	void setupWindow();
	GLFWwindow* getMainWindow() { return window; }

	// Get buffer dimensions
	inline int getBufferWidth() const { return bufferWidth; }
	inline int getBufferHeight() const { return bufferHeight; }

	inline uint32_t getWindowWidth() const { return windowWidth; }
	inline uint32_t getWindowHeight() const { return windowHeight; }

	inline bool getKeyPress(unsigned int keyVal) const { return keys[keyVal]; }

	inline GLFWwindow* getGlfwWindow() const { return window; }
	inline const Window* getCurrWindow() const { return this; }

	inline void setLMBPressed(bool val) { LMBPressed = val; }
	inline void setRMBPressed(bool val) { RMBPressed = val; }

	inline bool getLMBPressed() const { return LMBPressed; }
	inline bool getRMBPressed() const { return RMBPressed; }

	inline glm::vec2 getViewportCoord() const { return glm::vec2(viewportX, viewportY); }

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
		glDisable(GL_DEPTH_TEST);

		glfwDestroyWindow(window);
		glfwTerminate();
	}
};

#endif
