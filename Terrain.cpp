// Terrain.cpp

#include "Terrain.h"
#include "../Linker/include/stb/stb_image.h"
#include <iostream>


Terrain::Terrain()
    : width(0), height(0), heightScale(1.0f), horizontalScale(1.0f),
    terrainVAO(0), terrainVBO(0), terrainEBO(0), textureID(0),
    textureRepeat(10.0f), maxHeight(0.0f), terrainShader(nullptr)
{
}



Terrain::~Terrain() {
    cleanup();
}


void Terrain::setShader(Shader* shader) {
    terrainShader = shader;
}

Shader* Terrain::getShader() const {
    return terrainShader;
}


float Terrain::getMaxHeight() const {
    return maxHeight;
}

bool Terrain::loadHeightmap(const std::string& heightmapFile) {
    // Load heightmap image
    int nrComponents;
    unsigned char* data = stbi_load(heightmapFile.c_str(), &width, &height, &nrComponents, 1);
    if (!data) {
        std::cerr << "ERROR::TERRAIN::FAILED_TO_LOAD_HEIGHTMAP: " << heightmapFile << std::endl;
        return false;
    }

    // Process heightmap data
    heights.resize(width * height);
    maxHeight = 0.0f;
    for (int i = 0; i < width * height; ++i) {
        float heightValue = data[i] / 255.0f * heightScale;
        heights[i] = heightValue;
        if (heightValue > maxHeight) {
            maxHeight = heightValue;
        }
    }

    stbi_image_free(data);

    // Generate positions and normals
    positions.resize(width * height);
    normals.resize(width * height);
    texCoords.resize(width * height);

    float halfWidth = (width - 1) * horizontalScale * 0.5f;
    float halfDepth = (height - 1) * horizontalScale * 0.5f;

    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            float heightValue = heights[z * width + x];

            positions[z * width + x] = glm::vec3(
                x * horizontalScale - halfWidth,
                heightValue,
                z * horizontalScale - halfDepth
            );

            // Texture coordinates
            texCoords[z * width + x] = glm::vec2(
                static_cast<float>(x) / (width - 1) * textureRepeat,
                static_cast<float>(z) / (height - 1) * textureRepeat
            );
        }
    }

    calculateNormals();
    setupMesh();

    return true;
}

// Rest of Terrain.cpp remains the same as previously provided
// (For brevity, I will not repeat the code here, but make sure to include the full Terrain.cpp code from the previous response)


bool Terrain::loadTexture(const std::string& textureFile) {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Repeat wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Repeat wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Minification filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Magnification filter

    // Load image using stb_image
    int texWidth, texHeight, nrChannels;
    unsigned char* data = stbi_load(textureFile.c_str(), &texWidth, &texHeight, &nrChannels, 0);
    if (data) {
        GLenum format = GL_RGB;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
        std::cout << "INFO: Terrain texture loaded successfully." << std::endl;
        return true;
    }
    else {
        std::cerr << "ERROR::TERRAIN::FAILED_TO_LOAD_TEXTURE: " << textureFile << std::endl;
        stbi_image_free(data);
        return false;
    }
}

void Terrain::calculateNormals() {
    // Initialize normals to zero
    for (auto& normal : normals) {
        normal = glm::vec3(0.0f);
    }

    // Calculate normals
    for (int z = 0; z < height - 1; ++z) {
        for (int x = 0; x < width - 1; ++x) {
            int i0 = z * width + x;
            int i1 = z * width + x + 1;
            int i2 = (z + 1) * width + x;
            int i3 = (z + 1) * width + x + 1;

            glm::vec3 v0 = positions[i0];
            glm::vec3 v1 = positions[i1];
            glm::vec3 v2 = positions[i2];
            glm::vec3 v3 = positions[i3];

            glm::vec3 normal1 = glm::normalize(glm::cross(v1 - v0, v2 - v0));
            glm::vec3 normal2 = glm::normalize(glm::cross(v3 - v1, v2 - v1));

            normals[i0] += normal1;
            normals[i1] += normal1 + normal2;
            normals[i2] += normal1 + normal2;
            normals[i3] += normal2;
        }
    }

    // Normalize the normals
    for (auto& normal : normals) {
        normal = glm::normalize(normal);
    }
}

void Terrain::setupMesh() {
    // Generate indices
    indices.clear();
    for (int z = 0; z < height - 1; ++z) {
        for (int x = 0; x < width - 1; ++x) {
            int i0 = z * width + x;
            int i1 = z * width + x + 1;
            int i2 = (z + 1) * width + x;
            int i3 = (z + 1) * width + x + 1;

            // First triangle
            indices.push_back(i0);
            indices.push_back(i1);
            indices.push_back(i2);

            // Second triangle
            indices.push_back(i1);
            indices.push_back(i3);
            indices.push_back(i2);
        }
    }

    // Create buffers/arrays
    glGenVertexArrays(1, &terrainVAO);
    glGenBuffers(1, &terrainVBO);
    glGenBuffers(1, &terrainEBO);

    glBindVertexArray(terrainVAO);

    // Create vertex data
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    std::vector<Vertex> vertices;
    vertices.resize(positions.size());
    for (size_t i = 0; i < positions.size(); ++i) {
        vertices[i].Position = positions[i];
        vertices[i].Normal = normals[i];
        vertices[i].TexCoords = texCoords[i];
    }

    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Set the vertex attribute pointers
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // Normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // Texture coordinate attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}

// Terrain.cpp
void Terrain::render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPosition) {
    if (!terrainShader) {
        std::cerr << "ERROR: Terrain shader not set." << std::endl;
        return;
    }

    // Use the terrain shader
    terrainShader->use();

    // Set uniforms
    terrainShader->setMat4("model", model);
    terrainShader->setMat4("view", view);
    terrainShader->setMat4("projection", projection);
    terrainShader->setVec3("viewPos", cameraPosition);

    // Set lighting uniforms
    terrainShader->setVec3("lightPos", glm::vec3(0.0f, 100.0f, 0.0f)); // Adjust light position
    terrainShader->setFloat("shininess", 32.0f); // Adjust shininess

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    terrainShader->setInt("terrainTexture", 0);

    // Draw the terrain
    glBindVertexArray(terrainVAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


void Terrain::setHeightScale(float scale) {
    heightScale = scale;
}

void Terrain::setHorizontalScale(float scale) {
    horizontalScale = scale;
}

float Terrain::getHeightScale() const {
    return heightScale;
}

float Terrain::getHorizontalScale() const {
    return horizontalScale;
}

float Terrain::getHeightAtPosition(float x, float z) const {
    float halfWidth = (width - 1) * horizontalScale * 0.5f;
    float halfDepth = (height - 1) * horizontalScale * 0.5f;

    float localX = (x + halfWidth) / horizontalScale;
    float localZ = (z + halfDepth) / horizontalScale;

    // Clamp to grid boundaries
    localX = glm::clamp(localX, 0.0f, static_cast<float>(width - 1));
    localZ = glm::clamp(localZ, 0.0f, static_cast<float>(height - 1));

    // Get integer indices
    int x0 = static_cast<int>(localX);
    int z0 = static_cast<int>(localZ);
    int x1 = glm::min(x0 + 1, width - 1);
    int z1 = glm::min(z0 + 1, height - 1);

    // Fractional parts
    float fx = localX - x0;
    float fz = localZ - z0;

    // Heights at grid points
    float h00 = positions[z0 * width + x0].y;
    float h10 = positions[z0 * width + x1].y;
    float h01 = positions[z1 * width + x0].y;
    float h11 = positions[z1 * width + x1].y;

    // Bilinear interpolation
    float h0 = glm::mix(h00, h10, fx);
    float h1 = glm::mix(h01, h11, fx);
    float interpolatedHeight = glm::mix(h0, h1, fz);

    return interpolatedHeight;
}

void Terrain::cleanup() {
    if (terrainVAO != 0) {
        glDeleteVertexArrays(1, &terrainVAO);
        glDeleteBuffers(1, &terrainVBO);
        glDeleteBuffers(1, &terrainEBO);
        terrainVAO = 0;
        terrainVBO = 0;
        terrainEBO = 0;
    }
    if (textureID != 0) {
        glDeleteTextures(1, &textureID);
        textureID = 0;
    }
    positions.clear();
    normals.clear();
    indices.clear();
    heights.clear();
    texCoords.clear();
}

int Terrain::getWidth() const {
    return width;
}

int Terrain::getHeight() const {
    return height;
}
