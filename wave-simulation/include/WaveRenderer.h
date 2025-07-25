#ifndef WAVE_RENDERER_H
#define WAVE_RENDERER_H

#include <allegro5/allegro.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include "ShaderManager.h"

class WaveRenderer {
private:
    static const int GRID_SIZE = 100;
    static const int VERTEX_COUNT = GRID_SIZE * GRID_SIZE;
    
    GLuint VAO, VBO, EBO;
    ShaderManager* shaderManager;
    
    float* vertices;
    unsigned int* indices;
    int indexCount;
    
    float time;
    float waveSpeed;
    float waveHeight;
    float waveFrequency;
    
    // Mouse interaction
    float mouseX, mouseY;
    bool mousePressed;
    
    void generateMesh();
    void setupBuffers();
    void checkGLError(const std::string& location);

public:
    WaveRenderer();
    ~WaveRenderer();
    
    bool initialize();
    void update(float deltaTime);
    void render(int screenWidth, int screenHeight);
    
    void setMousePosition(float x, float y) { mouseX = x; mouseY = y; }
    void setMousePressed(bool pressed) { mousePressed = pressed; }
    void setWaveSpeed(float speed) { waveSpeed = speed; }
    void setWaveHeight(float height) { waveHeight = height; }
    void setWaveFrequency(float frequency) { waveFrequency = frequency; }
    bool loadWaveShaders();
};

#endif