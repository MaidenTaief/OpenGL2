// Terrain.h

#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Shader.h"

class Terrain {
public:
    Terrain();
    ~Terrain();

    bool loadHeightmap(const std::string& heightmapFile);
    bool loadTexture(const std::string& textureFile);

    void setHeightScale(float scale);
    void setHorizontalScale(float scale);
    float getHeightScale() const;
    float getHorizontalScale() const;

    float getHeightAtPosition(float x, float z) const;

    void render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPosition);

    void cleanup();

    int getWidth() const;
    int getHeight() const;

    void setShader(Shader* shader); // Accept a pointer
    Shader* getShader() const;      // Return a pointer

    float getMaxHeight() const; // Added getter for maximum height

private:
    int width;
    int height;
    float heightScale;
    float horizontalScale;

    std::vector<float> heights;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<unsigned int> indices;

    GLuint terrainVAO;
    GLuint terrainVBO;
    GLuint terrainEBO;

    GLuint textureID;

    Shader* terrainShader;

    float maxHeight; // Stores the maximum height value

    void calculateNormals();
    void setupMesh();

    float textureRepeat; // For texture coordinates
};
