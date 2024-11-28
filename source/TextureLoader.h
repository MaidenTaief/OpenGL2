// TextureLoader.h

#pragma once
#include <glad/glad.h>
#include <string>
#include <vector>

/**
 * @class TextureLoader
 * @brief Provides static methods to load textures and cubemaps.
 */
class TextureLoader {
public:
    /**
     * @brief Loads a 2D texture from a file.
     * @param path Path to the texture image.
     * @return OpenGL texture ID.
     */
    static GLuint loadTexture(const char* path);

    /**
     * @brief Loads a cubemap texture from 6 individual texture faces.
     * @param faces Vector containing paths to the 6 cubemap face images.
     * @return OpenGL cubemap texture ID.
     */
    static GLuint loadCubemap(const std::vector<std::string>& faces);
};
