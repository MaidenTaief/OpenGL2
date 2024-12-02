// main.cpp

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "WindowManager.h"
#include "Terrain.h"
#include "Hiker.h"
#include "Shader.h"
#include "log.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// Logging instance
Logger logger("application.log");

// Window dimensions
const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 720;

// Global variables for camera control
glm::vec3 cameraPosition = glm::vec3(0.0f, 100.0f, 200.0f);
glm::vec3 cameraFront = glm::normalize(-cameraPosition); // Looking towards the center
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
float yaw = -90.0f;
float pitch = 0.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Hiker model (as a simple cube)
GLuint hikerVAO = 0;
GLuint hikerVBO = 0;

// Callback for window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    if (width == 0 || height == 0) {
        // Avoid setting viewport to zero dimensions
        return;
    }
    glViewport(0, 0, width, height);
}

// Callback for mouse movement
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static float sensitivity = 0.1f; // Mouse sensitivity

    if (firstMouse) {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos) - lastX;
    float yoffset = lastY - static_cast<float>(ypos); // Reversed since y-coordinates go from bottom to top

    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Constrain the pitch angle
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // Update camera front vector
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// Process global input (e.g., ESC to close window)
void processInput(GLFWwindow* window) {
    float cameraSpeed = 50.0f * deltaTime; // Adjust accordingly

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
        logger.log("INFO: ESC key pressed. Closing the window.");
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPosition += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPosition -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// Function to initialize hiker model (a simple cube)
void initHikerModel() {
    float cubeVertices[] = {
        // positions          
        -0.5f,  0.0f, -0.5f,
         0.5f,  0.0f, -0.5f,
         0.5f,  1.0f, -0.5f,
         0.5f,  1.0f, -0.5f,
        -0.5f,  1.0f, -0.5f,
        -0.5f,  0.0f, -0.5f,

        -0.5f,  0.0f,  0.5f,
         0.5f,  0.0f,  0.5f,
         0.5f,  1.0f,  0.5f,
         0.5f,  1.0f,  0.5f,
        -0.5f,  1.0f,  0.5f,
        -0.5f,  0.0f,  0.5f,

        -0.5f,  1.0f,  0.5f,
        -0.5f,  1.0f, -0.5f,
        -0.5f,  0.0f, -0.5f,
        -0.5f,  0.0f, -0.5f,
        -0.5f,  0.0f,  0.5f,
        -0.5f,  1.0f,  0.5f,

         0.5f,  1.0f,  0.5f,
         0.5f,  1.0f, -0.5f,
         0.5f,  0.0f, -0.5f,
         0.5f,  0.0f, -0.5f,
         0.5f,  0.0f,  0.5f,
         0.5f,  1.0f,  0.5f,

        -0.5f,  0.0f, -0.5f,
         0.5f,  0.0f, -0.5f,
         0.5f,  0.0f,  0.5f,
         0.5f,  0.0f,  0.5f,
        -0.5f,  0.0f,  0.5f,
        -0.5f,  0.0f, -0.5f,

        -0.5f,  1.0f, -0.5f,
         0.5f,  1.0f, -0.5f,
         0.5f,  1.0f,  0.5f,
         0.5f,  1.0f,  0.5f,
        -0.5f,  1.0f,  0.5f,
        -0.5f,  1.0f, -0.5f,
    };

    glGenVertexArrays(1, &hikerVAO);
    glGenBuffers(1, &hikerVBO);

    glBindVertexArray(hikerVAO);

    glBindBuffer(GL_ARRAY_BUFFER, hikerVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

// Function to render the hiker at its current position
void renderHiker(const glm::vec3& position, Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
    shader.use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(2.0f)); // Adjust scale as needed

    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    shader.setVec3("objectColor", glm::vec3(1.0f, 0.0f, 0.0f)); // Red color

    glBindVertexArray(hikerVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

int main() {
    logger.log("INFO: Starting application");

    // Initialize WindowManager
    WindowManager windowManager(WIDTH, HEIGHT, "3D Hiking Simulator");
    GLFWwindow* window = windowManager.getWindow();

    // Set callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // Capture the mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Load terrain
    Terrain terrain;
    terrain.setHeightScale(50.0f);       // Adjust to make the mountain higher
    terrain.setHorizontalScale(1.0f);    // Adjust as needed
    if (!terrain.loadHeightmap("A:/Taief/semProVR/data/terrain.png")) {
        logger.log("ERROR: Failed to load terrain heightmap");
        return -1;
    }

    // Load terrain texture
    if (!terrain.loadTexture("A:/Taief/semProVR/textures/Terrain/Terrain005_1K_Color.png")) {
        logger.log("ERROR: Failed to load terrain texture");
        return -1;
    }

    // Load hiker path
    Hiker hiker("A:/Taief/semProVR/data/Afternoon_Run3.txt");
    hiker.setTerrain(&terrain);
    hiker.setScales(terrain.getHorizontalScale(), terrain.getHeightScale());
    hiker.setSpeed(10.0f); // Increase hiker speed
    if (!hiker.loadPathData(terrain)) {
        logger.log("ERROR: Failed to load hiker path data");
        return -1;
    }
    else {
        logger.log("INFO: Hiker path data loaded successfully.");
    }

    // Load shaders
    Shader terrainShader("A:/Taief/semProVR/shaders/terrainVert.glsl", "A:/Taief/semProVR/shaders/terrainFrag.glsl");
    Shader pathShader("A:/Taief/semProVR/shaders/pathVert.glsl", "A:/Taief/semProVR/shaders/pathFrag.glsl");
    Shader hikerShader("A:/Taief/semProVR/shaders/hikerVert.glsl", "A:/Taief/semProVR/shaders/hikerFrag.glsl"); // New shader for hiker

    if (!terrainShader.isLoaded() || !pathShader.isLoaded() || !hikerShader.isLoaded()) {
        logger.log("ERROR: Failed to load shaders");
        return -1;
    }

    // Pass terrain shader to terrain
    terrain.setShader(&terrainShader);

    // Initialize hiker model
    initHikerModel();

    // Set initial time
    lastFrame = static_cast<float>(glfwGetTime());

    logger.log("INFO: Starting main render loop");

    // Main render loop
    while (!windowManager.shouldClose()) {
        // Calculate deltaTime
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Clear buffers for the new frame
        glClearColor(0.6f, 0.8f, 1.0f, 1.0f); // Light blue sky color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Process input
        processInput(window);

        // Update camera front vector based on mouse movement
        cameraFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront.y = sin(glm::radians(pitch));
        cameraFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(cameraFront);

        // Camera/view transformation
        glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 1000.0f);

        // Render terrain
        terrain.render(glm::mat4(1.0f), view, projection, cameraPosition);

        // Update hiker's position
        hiker.updatePosition(deltaTime, terrain);

        // Render hiker's path
        hiker.renderPath(view, projection, pathShader);

        // Render hiker at current position
        glm::vec3 hikerPosition = hiker.getPosition();
        renderHiker(hikerPosition, hikerShader, view, projection);

        // Output hiker progress
        std::cout << "Hiker position: (" << hikerPosition.x << ", " << hikerPosition.y << ", " << hikerPosition.z << ")\n";

        // Swap buffers and poll for window events
        windowManager.swapBuffers();
        windowManager.pollEvents();
    }

    // Cleanup resources
    terrain.cleanup();
    hiker.cleanup();

    // Cleanup hiker model
    glDeleteVertexArrays(1, &hikerVAO);
    glDeleteBuffers(1, &hikerVBO);

    logger.log("INFO: Program terminated successfully");
    return 0;
}
