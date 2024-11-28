#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Terrain.h"
#include "Hiker.h"
#include "AnimatedCharacter.h"
#include "SeasonalEffect.h"
#include <memory>
#include "Lighting.h"
#include "Shader.h"
#include <glm/glm.hpp>

enum class CameraMode {
    OVERVIEW,
    FOLLOW,
    FIRST_PERSON
};

class HikingSimulator {
public:
    HikingSimulator();
    bool initialize();
    void processInput(GLFWwindow* window);
    void render(float deltaTime);
    void cleanup();
    void setWindowDimensions(int width, int height);
    void toggleSeason();
    void setCameraMode(CameraMode mode);
    CameraMode getCameraMode() const;
    const glm::mat4& getViewMatrix() const;
    const glm::mat4& getProjectionMatrix() const;
    void processMouseMovement(float xpos, float ypos);
    void processMouseButton(int button, int action);
    glm::vec3 getCameraFront() const { return cameraFront; }

private:
    void setupMatrices();
    void updateProjectionMatrix();
    void updateViewMatrix();
    void renderSkybox();

    float yaw = -90.0f;
    float pitch = 0.0f;
    float lastX = 0.0f;
    float lastY = 0.0f;
    bool firstMouse = true;
    bool isMouseEnabled = false;
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    Terrain terrain;
    Hiker hiker;
    AnimatedCharacter animatedCharacter;
    SeasonalEffect seasonalEffect;
    Lighting lighting;

    int width;
    int height;
    float windowWidth;
    float windowHeight;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 modelMatrix;
    glm::vec3 cameraPosition;

    std::unique_ptr<Shader> pathShader;
    float lastFrameTime;
    CameraMode cameraMode;
};
