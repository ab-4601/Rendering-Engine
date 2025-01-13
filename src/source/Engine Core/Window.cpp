#include "../../headers/Engine Core/Window.h"

Window::Window(int windowWidth, int windowHeight)
    : window{ nullptr }, windowWidth{ windowWidth }, windowHeight{ windowHeight }, bufferWidth{ 0 }, bufferHeight{ 0 },
    lastX{ 0.f }, lastY{ 0.f }, XChange{ 0.f }, YChange{ 0.f }, mouseFirstMoved{ true }, scrollChange{ 0.f },
    LMBPressed{ false }, RMBPressed{ false }, viewportX{ 0 }, viewportY{ 0 } {

    for (int i = 0; i < 1024; i++)
        keys[i] = false;

    setupWindow();
}

void Window::handleInput(GLFWwindow* window, int key, int code, int action, int mode) {
    Window* currWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            currWindow->keys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            currWindow->keys[key] = false;
        }
    }
}

void Window::setMousePosition(GLFWwindow* window, double xPos, double yPos) {
    GLFWwindow* temp = window;
    Window* currWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

    currWindow->viewportX = xPos;
    currWindow->viewportY = yPos;

    if (currWindow->getRMBPressed() == true) {
        glfwSetInputMode(temp, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        if (currWindow->mouseFirstMoved) {
            currWindow->lastX = (float)xPos;
            currWindow->lastY = (float)yPos;
            currWindow->mouseFirstMoved = false;
        }

        currWindow->XChange = static_cast<float>((float)xPos - currWindow->lastX);
        currWindow->YChange = static_cast<float>(currWindow->lastY - (float)yPos);

        currWindow->lastX = (float)xPos;
        currWindow->lastY = (float)yPos;
    }
    else {
        glfwSetInputMode(temp, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        currWindow->mouseFirstMoved = true;
    }
}

void Window::setMouseScroll(GLFWwindow* window, double xOffset, double yOffset) {
    Window* currWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if (currWindow->scrollChange < 15 || currWindow->scrollChange > -15)
        currWindow->scrollChange += (float)yOffset;
}

void Window::getMouseButton(GLFWwindow* window, int button, int action, int mods) {
    Window* currWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        currWindow->setLMBPressed(true);

    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        currWindow->setLMBPressed(false);

    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        currWindow->setRMBPressed(true);

    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        currWindow->setRMBPressed(false);
}

void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    Window* currWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

    currWindow->bufferWidth = width;
    currWindow->bufferHeight = height;

    //glfwGetFramebufferSize(currWindow->window, &currWindow->bufferWidth, &currWindow->bufferHeight);
    glfwGetWindowSize(currWindow->window, &currWindow->windowWidth, &currWindow->windowHeight);

    glViewport(0, 0, currWindow->bufferWidth, currWindow->bufferHeight);
}

void Window::windowFocusCallback(GLFWwindow* window, int focused) {
    Window* currWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

    currWindow->isWindowFocused = (bool)focused;
}

void Window::windowRefreshCallback(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glClearColor(0.f, 0.01f, 0.01f, 1.f);

    glfwSwapBuffers(window);
}

void Window::setupWindow() {
    if (!glfwInit()) {
        std::cerr << "Unable to initialize glfw" << std::endl;
        exit(1);
    }

    // Setup GLFW window properties
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, CONTEXT_VERSION::MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, CONTEXT_VERSION::MINOR);

    // Core profile = no backwards compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, ::samples);

    // Allow forward compatibility
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    if (windowWidth == 0 && windowHeight == 0) {
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

        windowWidth = mode->width;
        windowHeight = mode->height;

        window = glfwCreateWindow(mode->width, mode->height, "Main window", glfwGetPrimaryMonitor(), NULL);
    }
    else
        window = glfwCreateWindow(windowWidth, windowHeight, "Main window", NULL, NULL);

    if (!window) {
        std::cerr << "GLFW Window creation failed" << std::endl;
        glfwTerminate();
        exit(1);
    }

    glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);
    glfwMakeContextCurrent(window);
    createCallbacks();

    // Lock cursor to window
    //glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Allow modern extension features
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        GLenum result{};
        glewGetErrorString(result);
        std::cerr << "GLEW initialization failed\n" << result << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(1);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_CULL_FACE);

    glViewport(0, 0, bufferWidth, bufferHeight);

    glfwSetWindowUserPointer(window, this);
}