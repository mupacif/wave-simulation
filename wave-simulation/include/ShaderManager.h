#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include <string>
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

class ShaderManager {
private:
    GLuint programID;
    GLuint vertexShaderID;
    GLuint fragmentShaderID;

    std::string readShaderFile(const std::string& filepath);
    GLuint compileShader(const std::string& source, GLenum shaderType);
    void checkCompileErrors(GLuint shader, const std::string& type);
    void checkLinkErrors(GLuint program);

public:
    ShaderManager();
    ~ShaderManager();

    bool loadShaders(const std::string& vertexPath, const std::string& fragmentPath);
    void use() const;
    GLuint getProgramID() const { return programID; }
    
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setMat4(const std::string& name, const float* matrix) const;
};

#endif