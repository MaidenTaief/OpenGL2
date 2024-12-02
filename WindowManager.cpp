// WindowManager.cpp

#include "WindowManager.h"
#include <iostream>

/**
 * @brief Constructor that initializes the GLFW window.
 */
WindowManager::WindowManager(int width, int height, const char* title) {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "ERROR: Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Configure GLFW window settings
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS compatibility
#endif

    // Create GLFW window
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "ERROR: Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    // Set framebuffer size callback
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // Initialize GLAD before calling any OpenGL functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "ERROR: Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Configure OpenGL state
    glEnable(GL_DEPTH_TEST);

    // Set the viewport
    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);
}

/**
 * @brief Destructor that cleans up GLFW resources.
 */
WindowManager::~WindowManager() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

/**
 * @brief Checks if the window should close.
 */
bool WindowManager::shouldClose() {
    return glfwWindowShouldClose(window);
}

/**
 * @brief Swaps the front and back buffers.
 */
void WindowManager::swapBuffers() {
    glfwSwapBuffers(window);
}

/**
 * @brief Polls for and processes events.
 */
void WindowManager::pollEvents() {
    glfwPollEvents();
}

/**
 * @brief Retrieves the GLFW window pointer.
 */
GLFWwindow* WindowManager::getWindow() const {
    return window;
}

/**
 * @brief Callback function for framebuffer size changes.
 */
void WindowManager::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    if (width == 0 || height == 0) {
        // Avoid setting viewport to zero dimensions
        return;
    }
    glViewport(0, 0, width, height);
}
