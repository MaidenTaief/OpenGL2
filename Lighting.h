#ifndef LIGHTING_H
#define LIGHTING_H

#include <glm/glm.hpp>
#include "Shader.h"

class Lighting {
private:
    glm::vec3 position;
    glm::vec3 color;

public:
    // Constructor
    Lighting(const glm::vec3& pos = glm::vec3(1000.0f),
        const glm::vec3& col = glm::vec3(1.0f));

    // Inline getters
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getColor() const { return color; }

    // Shader application
    void apply(Shader& shader) const;
};

#endif // LIGHTING_H