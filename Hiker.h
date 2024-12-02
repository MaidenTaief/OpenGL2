// Hiker.h

#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Shader.h"
#include "Terrain.h"

/**
 * @brief Class representing a hiker moving along a path on the terrain.
 */
class Hiker {
public:
    /**
     * @brief Constructs a Hiker object with a path file.
     * @param pathFile Path to the file containing the hiker's path data.
     */
    Hiker(const std::string& pathFile);

    /**
     * @brief Sets scaling factors for horizontal and vertical axes.
     * @param hScale Horizontal scaling factor.
     * @param vScale Vertical scaling factor.
     */
    void setScales(float scale);
    void setScales(float hScale, float vScale);

    /**
     * @brief Sets the hiker's speed.
     * @param newSpeed The new speed value.
     */
    void setSpeed(float newSpeed);

    /**
     * @brief Loads path data from the file and validates it against the terrain.
     * @param terrain Reference to the terrain object.
     * @return True if the path data was loaded successfully, false otherwise.
     */
    bool loadPathData(const Terrain& terrain);

    /**
     * @brief Updates the hiker's position based on deltaTime.
     * @param deltaTime Time elapsed since the last update.
     * @param terrain Reference to the terrain object.
     */
    void updatePosition(float deltaTime, const Terrain& terrain);

    /**
     * @brief Renders the hiker's path.
     * @param view View matrix.
     * @param projection Projection matrix.
     * @param shader Shader used for rendering the path.
     */
    void renderPath(const glm::mat4& view, const glm::mat4& projection, Shader& shader);

    /**
     * @brief Cleans up OpenGL resources.
     */
    void cleanup();

    /**
     * @brief Gets the current position of the hiker.
     * @return Current position as a glm::vec3.
     */
    glm::vec3 getPosition() const;

    /**
     * @brief Sets the terrain reference for the hiker.
     * @param terrain Pointer to the terrain object.
     */
    void setTerrain(const Terrain* terrain);

    /**
     * @brief Moves the hiker forward along the path.
     * @param deltaTime Time elapsed since the last update.
     */
    void moveForward(float deltaTime);

    /**
     * @brief Moves the hiker backward along the path.
     * @param deltaTime Time elapsed since the last update.
     */
    void moveBackward(float deltaTime);

    /**
     * @brief Resets the hiker's path to the beginning.
     */
    void resetPath();

    /**
     * @brief Gets the path points.
     * @return Reference to the vector of path points.
     */
    const std::vector<glm::vec3>& getPathPoints() const;

private:
    // References
    const Terrain* terrainRef;    ///< Pointer to the terrain object.

    // Path data
    std::string pathFile;                 ///< Path to the file containing path data.
    std::vector<glm::vec3> pathPoints;    ///< List of points representing the path.
    std::vector<float> segmentDistances;  ///< Cumulative distances along the path.
    float totalPathLength;                ///< Total length of the path.
    float currentDistance;                ///< Current distance along the path.
    int currentSegmentIndex;              ///< Current segment index.
    float speed;                          ///< Hiker's speed along the path.

    bool movingForward;                   ///< Direction of movement

    // Scaling factors
    float horizontalScale;                ///< Scaling factor for horizontal axes.
    float heightScale;                    ///< Scaling factor for vertical axis.

    // Hiker state
    glm::vec3 position;                   ///< Current position of the hiker.

    // OpenGL resources
    GLuint pathVAO;                       ///< Vertex Array Object for the path.
    GLuint pathVBO;                       ///< Vertex Buffer Object for the path.

    // Helper functions
    void validatePath(const Terrain& terrain);
    void setupPathVAO();
    void calculateSegmentDistances();
};
