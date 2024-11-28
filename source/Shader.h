#pragma once

#include <string>
#include <unordered_map>
#include <glad/glad.h>
#include <glm/glm.hpp>

/**
 * @brief A class that encapsulates OpenGL shader program creation and usage.
 */
class Shader {
public:
    /**
     * @brief Constructs a Shader object by loading and compiling vertex and fragment shaders.
     * @param vertexPath Path to the vertex shader source file.
     * @param fragmentPath Path to the fragment shader source file.
     */
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    /**
     * @brief Destructor that cleans up the shader program.
     */
    ~Shader();

    /**
     * @brief Activates the shader program for use.
     */
    void use() const;

    /**
     * @brief Checks if the shader program was loaded successfully.
     * @return True if loaded successfully, false otherwise.
     */
    bool isLoaded() const;

    // Uniform setters
    void setMat4(const std::string& name, const glm::mat4& matrix) const;
    void setVec3(const std::string& name, const glm::vec3& vector) const;
    void setFloat(const std::string& name, float value) const;
    void setInt(const std::string& name, int value) const;

private:
    GLuint programID;  ///< OpenGL ID for the shader program.
    bool loaded;       ///< Flag indicating if the shader was loaded successfully.
    mutable std::unordered_map<std::string, GLint> uniformCache; ///< Cache for uniform locations.

    // Helper functions
    std::string loadShaderSource(const std::string& filepath) const;
    GLuint compileShader(const char* source, GLenum shaderType) const;
    void checkCompileErrors(GLuint shader, const std::string& type) const;
    GLint getUniformLocation(const std::string& name) const;
};

