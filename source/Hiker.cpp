// Hiker.cpp

#include "Hiker.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>

Hiker::Hiker(const std::string& pathFile)
    : terrainRef(nullptr), pathFile(pathFile),
    totalPathLength(0.0f), currentDistance(0.0f), currentSegmentIndex(0),
    speed(5.0f), horizontalScale(1.0f), heightScale(1.0f),
    position(glm::vec3(0.0f)), pathVAO(0), pathVBO(0),
    movingForward(true)
{
}

void Hiker::setScales(float scale) {
    horizontalScale = scale;
    heightScale = scale;
}

void Hiker::setScales(float hScale, float vScale) {
    horizontalScale = hScale;
    heightScale = vScale;
}

void Hiker::setSpeed(float newSpeed) {
    speed = newSpeed;
}

void Hiker::setTerrain(const Terrain* terrain) {
    terrainRef = terrain;
}

bool Hiker::loadPathData(const Terrain& terrain) {
    std::ifstream file(pathFile);
    if (!file.is_open()) {
        std::cerr << "ERROR::HIKER::FAILED_TO_OPEN_PATH_FILE: " << pathFile << std::endl;
        return false;
    }

    float x, y, z;
    pathPoints.clear();

    // Load path points from file
    while (file >> x >> y >> z) {
        pathPoints.emplace_back(x, y, z);
    }
    file.close();

    if (pathPoints.empty()) {
        std::cerr << "ERROR::HIKER::NO_PATH_POINTS_LOADED" << std::endl;
        return false;
    }

    // Validate and adjust the path points against the terrain
    validatePath(terrain);

    // Calculate segment distances and total path length
    calculateSegmentDistances();

    position = pathPoints[0];
    currentDistance = 0.0f;
    currentSegmentIndex = 0;

    // Setup OpenGL buffers for rendering the path
    setupPathVAO();

    return true;
}

void Hiker::validatePath(const Terrain& terrain) {
    if (pathPoints.empty()) return;

    std::vector<glm::vec3> validatedPath;
    validatedPath.reserve(pathPoints.size());

    float terrainWidth = terrain.getWidth() * terrain.getHorizontalScale();
    float terrainDepth = terrain.getHeight() * terrain.getHorizontalScale();
    float minX = -terrainWidth * 0.5f;
    float maxX = terrainWidth * 0.5f;
    float minZ = -terrainDepth * 0.5f;
    float maxZ = terrainDepth * 0.5f;

    // Scale and adjust points
    for (auto& point : pathPoints) {
        point.x = glm::clamp(point.x * horizontalScale, minX, maxX);
        point.z = glm::clamp(point.z * horizontalScale, minZ, maxZ);
        float terrainHeight = terrain.getHeightAtPosition(point.x, point.z);
        point.y = terrainHeight + 0.5f; // Small offset above terrain
        validatedPath.push_back(point);
    }

    pathPoints = std::move(validatedPath);
}

void Hiker::calculateSegmentDistances() {
    segmentDistances.clear();
    segmentDistances.push_back(0.0f);
    totalPathLength = 0.0f;

    for (size_t i = 1; i < pathPoints.size(); ++i) {
        float segmentLength = glm::distance(pathPoints[i - 1], pathPoints[i]);
        totalPathLength += segmentLength;
        segmentDistances.push_back(totalPathLength);
    }
}

void Hiker::setupPathVAO() {
    // Cleanup previous buffers if any
    if (pathVAO != 0) {
        glDeleteVertexArrays(1, &pathVAO);
        glDeleteBuffers(1, &pathVBO);
    }

    // Generate and bind VAO and VBO
    glGenVertexArrays(1, &pathVAO);
    glGenBuffers(1, &pathVBO);

    glBindVertexArray(pathVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pathVBO);
    glBufferData(GL_ARRAY_BUFFER, pathPoints.size() * sizeof(glm::vec3), pathPoints.data(), GL_STATIC_DRAW);

    // Vertex attribute setup
    glEnableVertexAttribArray(0); // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindVertexArray(0);
}

void Hiker::updatePosition(float deltaTime, const Terrain& terrain) {
    // Advance along the path based on speed and deltaTime
    float distanceToMove = speed * deltaTime;

    currentDistance += distanceToMove;

    if (currentDistance >= totalPathLength) {
        currentDistance = 0.0f; // Reset to start for looping
        currentSegmentIndex = 0;
    }

    if (movingForward) {
        currentDistance += distanceToMove;
        if (currentDistance >= totalPathLength) {
            currentDistance = totalPathLength;
            movingForward = false; // Change direction or stop at the end
        }
    }
    else {
        currentDistance -= distanceToMove;
        if (currentDistance <= 0.0f) {
            currentDistance = 0.0f;
            movingForward = true; // Change direction or stop at the start
        }
    }

    // Find the segment of the path we're currently on
    while (currentSegmentIndex < segmentDistances.size() - 1 &&
        currentDistance > segmentDistances[currentSegmentIndex + 1]) {
        currentSegmentIndex++;
    }
    while (currentSegmentIndex > 0 &&
        currentDistance < segmentDistances[currentSegmentIndex]) {
        currentSegmentIndex--;
    }

    // Calculate the interpolation factor 't' for the current segment
    float segmentStartDistance = segmentDistances[currentSegmentIndex];
    float segmentEndDistance = segmentDistances[currentSegmentIndex + 1];
    float segmentLength = segmentEndDistance - segmentStartDistance;
    float t = (currentDistance - segmentStartDistance) / segmentLength;

    // Interpolate between the start and end points of the segment
    glm::vec3 startPoint = pathPoints[currentSegmentIndex];
    glm::vec3 endPoint = pathPoints[currentSegmentIndex + 1];
    glm::vec3 interpolatedPosition = glm::mix(startPoint, endPoint, t);

    // Get the terrain height at the current (X, Z) position
    float terrainHeight = terrain.getHeightAtPosition(interpolatedPosition.x, interpolatedPosition.z);
    interpolatedPosition.y = terrainHeight + 0.5f; // Small offset above terrain

    // Update the hiker's position
    position = interpolatedPosition;
}

void Hiker::moveForward(float deltaTime) {
    movingForward = true;
    if (terrainRef) {
        updatePosition(deltaTime, *terrainRef);
    }
}

void Hiker::moveBackward(float deltaTime) {
    movingForward = false;
    if (terrainRef) {
        updatePosition(deltaTime, *terrainRef);
    }
}

void Hiker::resetPath() {
    currentDistance = 0.0f;
    currentSegmentIndex = 0;
    movingForward = true;
    if (!pathPoints.empty()) {
        position = pathPoints[0];
    }
}

const std::vector<glm::vec3>& Hiker::getPathPoints() const {
    return pathPoints;
}

void Hiker::renderPath(const glm::mat4& view, const glm::mat4& projection, Shader& shader) {
    shader.use();
    shader.setMat4("model", glm::mat4(1.0f));
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Draw the path
    glLineWidth(2.0f);
    shader.setVec3("pathColor", glm::vec3(1.0f, 0.0f, 0.0f));
    glBindVertexArray(pathVAO);
    glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(pathPoints.size()));

    glLineWidth(1.0f);
    glBindVertexArray(0);
}

void Hiker::cleanup() {
    if (pathVAO != 0) {
        glDeleteVertexArrays(1, &pathVAO);
        glDeleteBuffers(1, &pathVBO);
        pathVAO = 0;
        pathVBO = 0;
    }
    pathPoints.clear();
}

glm::vec3 Hiker::getPosition() const {
    return position;
}
