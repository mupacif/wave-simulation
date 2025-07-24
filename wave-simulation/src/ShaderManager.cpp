#include "ShaderManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

ShaderManager::ShaderManager() : programID(0), vertexShaderID(0), fragmentShaderID(0) {
}

ShaderManager::~ShaderManager() {
    if (programID) {
        glDeleteProgram(programID);
    }
}

std::string ShaderManager::readShaderFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filepath << std::endl;
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint ShaderManager::compileShader(const std::string& source, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);
    const char* sourceCStr = source.c_str();
    glShaderSource(shader, 1, &sourceCStr, nullptr);
    glCompileShader(shader);
    
    checkCompileErrors(shader, shaderType == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");
    
    return shader;
}

void ShaderManager::checkCompileErrors(GLuint shader, const std::string& type) {
    GLint success;
    GLchar infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cerr << "Shader compilation error (" << type << "): " << infoLog << std::endl;
    }
}

void ShaderManager::checkLinkErrors(GLuint program) {
    GLint success;
    GLchar infoLog[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, nullptr, infoLog);
        std::cerr << "Shader linking error: " << infoLog << std::endl;
    }
}

bool ShaderManager::loadShaders(const std::string& vertexPath, const std::string& fragmentPath) {
    std::cout << "Loading shaders from: " << vertexPath << " and " << fragmentPath << std::endl;
    
    // Read shader files
    std::string vertexSource = readShaderFile(vertexPath);
    std::string fragmentSource = readShaderFile(fragmentPath);
    
    if (vertexSource.empty() || fragmentSource.empty()) {
        std::cerr << "Failed to read shader files" << std::endl;
        return false;
    }
    
    std::cout << "Shader files read successfully" << std::endl;
    
    // Compile shaders
    vertexShaderID = compileShader(vertexSource, GL_VERTEX_SHADER);
    fragmentShaderID = compileShader(fragmentSource, GL_FRAGMENT_SHADER);
    
    // Create program and link shaders
    programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);
    
    checkLinkErrors(programID);
    
    // Delete shaders as they're linked into the program now
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
    
    return true;
}

void ShaderManager::use() const {
    glUseProgram(programID);
}

void ShaderManager::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}

void ShaderManager::setVec2(const std::string& name, float x, float y) const {
    glUniform2f(glGetUniformLocation(programID, name.c_str()), x, y);
}

void ShaderManager::setVec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(programID, name.c_str()), x, y, z);
}

void ShaderManager::setMat4(const std::string& name, const float* matrix) const {
    glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, matrix);
}