#include "Lighting.h"
#include <glm/gtc/type_ptr.hpp>

Lighting::Lighting(const glm::vec3& pos, const glm::vec3& col)
    : position(pos), color(col) {
}

void Lighting::apply(Shader& shader) const {
    shader.setVec3("lightPos", position);
    shader.setVec3("lightColor", color);

    // Add enhanced lighting parameters
    shader.setVec3("light.position", position);
    shader.setVec3("light.ambient", color * 0.3f);
    shader.setVec3("light.diffuse", color);
    shader.setVec3("light.specular", color * 0.5f);
}