// WindowManager.h

#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

// Include glad.h before glfw3.h
#include <glad/glad.h>

// Prevent GLFW from including OpenGL headers
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <string>

/**
 * @class WindowManager
 * @brief Manages the creation and handling of the GLFW window.
 */
class WindowManager {
public:
    /**
     * @brief Constructor that initializes the GLFW window.
     * @param width Width of the window.
     * @param height Height of the window.
     * @param title Title of the window.
     */
    WindowManager(int width, int height, const char* title);

    /**
     * @brief Destructor that cleans up GLFW resources.
     */
    ~WindowManager();

    /**
     * @brief Checks if the window should close.
     * @return True if the window should close, false otherwise.
     */
    bool shouldClose();

    /**
     * @brief Swaps the front and back buffers.
     */
    void swapBuffers();

    /**
     * @brief Polls for and processes events.
     */
    void pollEvents();

    /**
     * @brief Retrieves the GLFW window pointer.
     * @return Pointer to the GLFWwindow.
     */
    GLFWwindow* getWindow() const;

    /**
     * @brief Callback function for framebuffer size changes.
     * @param window Pointer to the GLFWwindow.
     * @param width New width of the framebuffer.
     * @param height New height of the framebuffer.
     */
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

private:
    GLFWwindow* window; ///< Pointer to the GLFW window.
};

#endif // WINDOWMANAGER_H
