#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

// Enable shader debugging output
constexpr bool SHADER_DEBUG = true;

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
    : programID(0), loaded(false) {

    if (SHADER_DEBUG) {
        std::cout << "INFO::SHADER::CREATING_SHADER: Vertex(" << vertexPath
            << ") Fragment(" << fragmentPath << ")\n";
    }

    // Load shader source code from files
    std::string vertexCode = loadShaderSource(vertexPath);
    std::string fragmentCode = loadShaderSource(fragmentPath);

    if (vertexCode.empty() || fragmentCode.empty()) {
        std::cerr << "ERROR::SHADER::FAILED_TO_LOAD_SHADER_SOURCE\n";
        return;
    }

    // Compile shaders
    GLuint vertexShader = compileShader(vertexCode.c_str(), GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentCode.c_str(), GL_FRAGMENT_SHADER);

    if (!vertexShader || !fragmentShader) {
        std::cerr << "ERROR::SHADER::SHADER_COMPILATION_FAILED\n";
        return;
    }

    // Link shaders into a program
    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);
    checkCompileErrors(programID, "PROGRAM");

    // Delete shader objects after linking
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    loaded = true;

    if (SHADER_DEBUG) {
        std::cout << "INFO::SHADER::PROGRAM_CREATED_SUCCESSFULLY\n";
    }
}

Shader::~Shader() {
    if (programID > 0) {
        glDeleteProgram(programID);
    }
}

void Shader::use() const {
    if (loaded) {
        glUseProgram(programID);
    }
    else {
        std::cerr << "ERROR::SHADER::PROGRAM_NOT_LOADED\n";
    }
}

bool Shader::isLoaded() const {
    return loaded;
}

void Shader::setMat4(const std::string& name, const glm::mat4& matrix) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }
}

void Shader::setVec3(const std::string& name, const glm::vec3& vector) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform3fv(location, 1, glm::value_ptr(vector));
    }
}

void Shader::setFloat(const std::string& name, float value) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform1f(location, value);
    }
}

void Shader::setInt(const std::string& name, int value) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform1i(location, value);
    }
}

std::string Shader::loadShaderSource(const std::string& filepath) const {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "ERROR::SHADER::COULD_NOT_OPEN_FILE: " << filepath << "\n";
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    if (SHADER_DEBUG) {
        std::cout << "INFO::SHADER::LOADED_SOURCE_FROM: " << filepath << "\n";
    }

    return buffer.str();
}

GLuint Shader::compileShader(const char* source, GLenum shaderType) const {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    checkCompileErrors(shader, shaderType == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        // Compilation failed; delete shader
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

void Shader::checkCompileErrors(GLuint shader, const std::string& type) const {
    GLint success;
    std::vector<GLchar> infoLog(1024);

    if (type != "PROGRAM") {
        // Shader compilation errors
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, static_cast<GLsizei>(infoLog.size()), nullptr, infoLog.data());
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                << infoLog.data() << "\n";
        }
    }
    else {
        // Program linking errors
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, static_cast<GLsizei>(infoLog.size()), nullptr, infoLog.data());
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR\n" << infoLog.data() << "\n";
        }
    }
}

GLint Shader::getUniformLocation(const std::string& name) const {
    // Check if the uniform location is cached
    auto it = uniformCache.find(name);
    if (it != uniformCache.end()) {
        return it->second;
    }

    // Retrieve the location from OpenGL
    GLint location = glGetUniformLocation(programID, name.c_str());
    if (location == -1 && SHADER_DEBUG) {
        std::cerr << "WARNING::SHADER::UNIFORM_NOT_FOUND: " << name << "\n";
    }

    // Cache the location for future use
    uniformCache[name] = location;
    return location;
}
