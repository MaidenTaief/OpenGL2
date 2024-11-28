#include "HikingSimulator.h"
#include "Skybox.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <cmath>

// Constructor
HikingSimulator::HikingSimulator()
    : terrain(),
    hiker("A:/Taief/semProVR/data/Afternoon_Run3.txt"),
    animatedCharacter(),
    lighting(glm::vec3(1000.0f, 1000.0f, 1000.0f), glm::vec3(1.0f, 0.95f, 0.8f)),
    width(0),
    height(0),
    seasonalEffect(),
    viewMatrix(glm::mat4(1.0f)),
    projectionMatrix(glm::mat4(1.0f)),
    modelMatrix(glm::mat4(1.0f)),
    cameraPosition(glm::vec3(0.0f)),
    windowWidth(800.0f),
    windowHeight(600.0f),
    pathShader(nullptr),
    lastFrameTime(0.0f),
    cameraMode(CameraMode::OVERVIEW),
    isMouseEnabled(false),
    firstMouse(true),
    yaw(-90.0f),
    pitch(0.0f),
    lastX(0.0f),
    lastY(0.0f)
{
}

void HikingSimulator::setWindowDimensions(int width, int height) {
    this->windowWidth = static_cast<float>(width);
    this->windowHeight = static_cast<float>(height);
    updateProjectionMatrix();
}

bool HikingSimulator::initialize() {
    std::cout << "INFO: Initializing HikingSimulator..." << std::endl;

    // Load terrain heightmap
    if (!terrain.loadHeightmap("data/terrain_heightmap.png")) {
        std::cerr << "ERROR: Failed to load terrain heightmap" << std::endl;
        return false;
    }

    // Set scales and terrain reference for the hiker
    hiker.setScales(1.0f);
    hiker.setTerrain(&terrain);

    width = terrain.getWidth();
    height = terrain.getHeight();

    if (width <= 0 || height <= 0) {
        std::cerr << "ERROR: Invalid terrain dimensions!" << std::endl;
        return false;
    }

    std::cout << "INFO: Terrain dimensions: " << width << " x " << height << std::endl;
    std::cout << "INFO: Terrain scales - Horizontal: " << terrain.getHorizontalScale()
        << ", Height: " << terrain.getHeightScale() << std::endl;

    hiker.setScales(terrain.getHorizontalScale(), terrain.getHeightScale());

    // Initialize skybox
    Skybox& skybox = Skybox::getInstance();
    if (!skybox.initialize("textures/skybox/")) {
        std::cerr << "ERROR: Failed to initialize skybox!" << std::endl;
        return false;
    }

    // Load hiker path data
    if (!hiker.loadPathData(terrain)) {
        std::cerr << "ERROR: Failed to load hiker path!" << std::endl;
        return false;
    }
    else {
        std::cout << "INFO: Hiker path loaded successfully." << std::endl;
    }

    // Load shaders
    pathShader = std::make_unique<Shader>("A:/Taief/semProVR/shaders/pathVert.glsl", "A:/Taief/semProVR/shaders/pathFrag.glsl");
    if (!pathShader || !pathShader->isLoaded()) {
        std::cerr << "ERROR: Failed to load path shader during initialization." << std::endl;
        return false;
    }

    // Initialize seasonal effect
    seasonalEffect.initialize(SeasonalEffect::Season::NONE);
    setupMatrices();

    // Load animated character path data
    animatedCharacter.loadPathData(hiker.getPathPoints());

    lastFrameTime = static_cast<float>(glfwGetTime());

    std::cout << "INFO: HikingSimulator initialized successfully." << std::endl;
    return true;
}

void HikingSimulator::setupMatrices() {
    updateProjectionMatrix();
    updateViewMatrix();
}

void HikingSimulator::updateViewMatrix() {
    float hScale = terrain.getHorizontalScale() * 6.0f;
    float terrainWidth = width * hScale;
    float terrainDepth = height * hScale;
    float maxTerrainHeight = terrain.getMaxHeight();

    switch (cameraMode) {
    case CameraMode::OVERVIEW: {
        float aspectRatio = windowWidth / windowHeight;
        float viewDistance = std::max(terrainWidth, terrainDepth) * 0.5f;
        float viewHeight = maxTerrainHeight * 2.5f;

        cameraPosition = glm::vec3(
            0.0f,
            viewHeight,
            viewDistance
        );

        glm::vec3 target(0.0f, 0.0f, 0.0f);

        viewMatrix = glm::lookAt(cameraPosition, target, glm::vec3(0.0f, 1.0f, 0.0f));
        break;
    }
    case CameraMode::FOLLOW: {
        glm::vec3 hikerPos = hiker.getPosition();
        float cameraHeight = maxTerrainHeight * 0.2f;
        float cameraDistance = 50.0f;

        cameraPosition = hikerPos + glm::vec3(0.0f, cameraHeight, cameraDistance);
        glm::vec3 target = hikerPos;
        viewMatrix = glm::lookAt(cameraPosition, target, cameraUp);
        break;
    }
    case CameraMode::FIRST_PERSON: {
        glm::vec3 hikerPos = hiker.getPosition();
        cameraPosition = hikerPos + glm::vec3(0.0f, 2.0f, 0.0f);
        viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
        break;
    }
    }
}

void HikingSimulator::updateProjectionMatrix() {
    float aspectRatio = windowWidth / windowHeight;
    float verticalFOV = 50.0f;  // Wider FOV for better coverage
    float hScale = terrain.getHorizontalScale() * 10.0f;
    float viewDistance = std::max(width * hScale, height * hScale);

    projectionMatrix = glm::perspective(
        glm::radians(verticalFOV),
        aspectRatio,
        0.1f,
        viewDistance * 2.0f
    );
}

void HikingSimulator::processInput(GLFWwindow* window) {
    float currentTime = static_cast<float>(glfwGetTime());
    float deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;

    // Camera mode toggles
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        cameraMode = CameraMode::OVERVIEW;
        isMouseEnabled = false;
        updateViewMatrix();
    }
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        cameraMode = CameraMode::FOLLOW;
        isMouseEnabled = true;
        updateViewMatrix();
    }
    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        cameraMode = CameraMode::FIRST_PERSON;
        isMouseEnabled = true;
        updateViewMatrix();
    }

    // Movement controls
    if (cameraMode != CameraMode::OVERVIEW) {
        float moveSpeed = 20.0f * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            animatedCharacter.moveForward(deltaTime);
            hiker.moveForward(deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            animatedCharacter.moveBackward(deltaTime);
            hiker.moveBackward(deltaTime);
        }

        // Strafe controls
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            glm::vec3 right = glm::normalize(glm::cross(cameraFront, cameraUp));
            cameraPosition -= right * moveSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            glm::vec3 right = glm::normalize(glm::cross(cameraFront, cameraUp));
            cameraPosition += right * moveSpeed;
        }
    }

    // Other controls
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        animatedCharacter.resetHike();
        hiker.resetPath();
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        isMouseEnabled = !isMouseEnabled;
    }

    updateViewMatrix();
}

void HikingSimulator::processMouseMovement(float xpos, float ypos) {
    if (!isMouseEnabled) return;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

void HikingSimulator::render(float deltaTime) {
    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Render skybox first
    glDepthFunc(GL_LEQUAL);
    glm::mat4 skyboxView = glm::mat4(glm::mat3(viewMatrix)); // Remove translation
    Skybox::getInstance().render(skyboxView, projectionMatrix);
    glDepthFunc(GL_LESS);

    // Enable face culling for terrain
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Update positions
    hiker.updatePosition(deltaTime, terrain);
    animatedCharacter.updatePosition(deltaTime, terrain);

    if (cameraMode != CameraMode::OVERVIEW) {
        updateViewMatrix();
    }

    // Render terrain
    terrain.render(modelMatrix, viewMatrix, projectionMatrix, cameraPosition);

    // Disable face culling for transparent objects
    glDisable(GL_CULL_FACE);

    // Render path
    if (pathShader && pathShader->isLoaded()) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);

        pathShader->use();
        pathShader->setMat4("model", modelMatrix);
        pathShader->setMat4("view", viewMatrix);
        pathShader->setMat4("projection", projectionMatrix);
        pathShader->setFloat("heightOffset", 0.05f); // Minimal height offset
        pathShader->setVec3("pathColor", glm::vec3(1.0f, 0.0f, 0.0f));

        hiker.renderPath(viewMatrix, projectionMatrix, *pathShader);

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }

    // Render character and effects
    animatedCharacter.render(viewMatrix, projectionMatrix, *pathShader);
    seasonalEffect.render(viewMatrix, projectionMatrix);
}

void HikingSimulator::cleanup() {
    terrain.cleanup();
    hiker.cleanup();
    animatedCharacter.cleanup();
    Skybox::getInstance().cleanup();
    seasonalEffect.cleanup();
    std::cout << "INFO: HikingSimulator cleaned up successfully." << std::endl;
}

void HikingSimulator::toggleSeason() {
    static SeasonalEffect::Season currentSeason = SeasonalEffect::Season::NONE;
    currentSeason = static_cast<SeasonalEffect::Season>(
        (static_cast<int>(currentSeason) + 1) % 3
        );
    seasonalEffect.setSeason(currentSeason);
}

const glm::mat4& HikingSimulator::getViewMatrix() const {
    return viewMatrix;
}

const glm::mat4& HikingSimulator::getProjectionMatrix() const {
    return projectionMatrix;
}

void HikingSimulator::setCameraMode(CameraMode mode) {
    cameraMode = mode;
    updateViewMatrix();
}

CameraMode HikingSimulator::getCameraMode() const {
    return cameraMode;
}

void HikingSimulator::processMouseButton(int button, int action) {
    // Implement mouse button processing if needed
}
