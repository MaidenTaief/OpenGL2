#ifndef ANIMATED_CHARACTER_H
#define ANIMATED_CHARACTER_H

#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"
#include "Terrain.h"

class AnimatedCharacter {
private:
    GLuint characterVAO, characterVBO;         // OpenGL buffers for the character model
    glm::vec3 characterPosition;               // Current position of the character
    std::vector<glm::vec3> pathPoints;         // Path points for the animation
    float progress;                            // Animation progress (0.0 - 1.0)
    int currentPathIndex;                      // Current index along the path
    bool movingForward;                        // Animation direction (forward or backward)
    float movementSpeed;                       // Speed of character movement
    float totalPathLength;                     // Total path length
    float distanceHiked;                       // Distance covered
    float distanceRemaining;                   // Distance left to hike
    float timeElapsed;                         // Time since the hike started
    float elevationChange;                     // Total elevation change

    void setupCharacterBuffers();              // Initialize character buffers
    float calculatePathLength();               // Helper function to calculate total path length
    float calculateElevationChange();          // Helper function to calculate total elevation change

public:
    AnimatedCharacter();                       // Constructor
    ~AnimatedCharacter();                      // Destructor

    void moveForward(float deltaTime);
    void moveBackward(float deltaTime);

    void loadPathData(const std::vector<glm::vec3>& path);  // Load path points
    void updatePosition(float deltaTime, const Terrain& terrain); // Update character position
    void render(const glm::mat4& view, const glm::mat4& projection, Shader& shader); // Render the character
    void resetHike();                          // Reset hike stats
    void cleanup();                            // Cleanup OpenGL resources

    // Getters for attributes
    float getDistanceHiked() const;
    float getDistanceRemaining() const;
    float getTimeElapsed() const;
    float getElevationChange() const;
    glm::vec3 getCurrentPosition() const;
};

#endif // ANIMATED_CHARACTER_H
