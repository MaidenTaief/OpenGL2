// TextureLoader.cpp

#include "TextureLoader.h"
#include "../Linker/include/stb/stb_image.h"
#include <iostream>

/**
 * @brief Loads a 2D texture from a file.
 */
GLuint TextureLoader::loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;

    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format = 0;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        if (format == 0) {
            std::cerr << "ERROR: Unknown number of channels in texture: " << path << std::endl;
            stbi_image_free(data);
            glDeleteTextures(1, &textureID);
            return 0;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
            0, format, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
            format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
            format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
            GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
            GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

/**
 * @brief Loads a cubemap texture from 6 individual texture faces.
 */
GLuint TextureLoader::loadCubemap(const std::vector<std::string>& faces) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false); // Prevent flipping for cubemaps
    for (GLuint i = 0; i < faces.size(); i++) {
        unsigned char* data =
            stbi_load(faces[i].c_str(), &width, &height,
                &nrChannels, 0);
        if (data) {
            GLenum format = 0;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;

            if (format == 0) {
                std::cerr << "ERROR: Unknown number of channels in cubemap texture: " << faces[i] << std::endl;
                stbi_image_free(data);
                glDeleteTextures(1, &textureID);
                return 0;
            }

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                format, width, height, 0, format,
                GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
            glDeleteTextures(1, &textureID); // Clean up partially loaded texture
            return 0;
        }
    }

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Prevent seams
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
