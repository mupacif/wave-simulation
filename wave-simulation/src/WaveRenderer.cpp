#include "WaveRenderer.h"
#include <cmath>
#include <cstring>
#include <iostream>

WaveRenderer::WaveRenderer() 
    : VAO(0), VBO(0), EBO(0), shaderManager(nullptr),
      vertices(nullptr), indices(nullptr), indexCount(0),
      time(0.0f), waveSpeed(1.0f), waveHeight(0.2f), waveFrequency(5.0f),
      mouseX(0.0f), mouseY(0.0f), mousePressed(false) {
}

WaveRenderer::~WaveRenderer() {
    delete shaderManager;
    delete[] vertices;
    delete[] indices;
    
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
}

void WaveRenderer::generateMesh() {
    // Generate vertices
    vertices = new float[VERTEX_COUNT * 3];
    int idx = 0;
    
    float step = 2.0f / (GRID_SIZE - 1);
    for (int z = 0; z < GRID_SIZE; z++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            vertices[idx++] = x * step - 1.0f;  // X position [-1, 1]
            vertices[idx++] = 0.0f;              // Y position (will be modified by shader)
            vertices[idx++] = z * step - 1.0f;  // Z position [-1, 1]
        }
    }
    
    // Generate indices for triangle strips
    indexCount = (GRID_SIZE - 1) * (GRID_SIZE - 1) * 6;
    indices = new unsigned int[indexCount];
    idx = 0;
    
    for (int z = 0; z < GRID_SIZE - 1; z++) {
        for (int x = 0; x < GRID_SIZE - 1; x++) {
            unsigned int topLeft = z * GRID_SIZE + x;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (z + 1) * GRID_SIZE + x;
            unsigned int bottomRight = bottomLeft + 1;
            
            // First triangle
            indices[idx++] = topLeft;
            indices[idx++] = bottomLeft;
            indices[idx++] = topRight;
            
            // Second triangle
            indices[idx++] = topRight;
            indices[idx++] = bottomLeft;
            indices[idx++] = bottomRight;
        }
    }
}

void WaveRenderer::setupBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, VERTEX_COUNT * 3 * sizeof(float), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
}

bool WaveRenderer::initialize() {
    // Create shader manager
    shaderManager = new ShaderManager();
    if (!shaderManager->loadShaders("shaders/wave.vert", "shaders/wave.frag")) {
        std::cerr << "Failed to load shaders" << std::endl;
        return false;
    }
    
    // Generate mesh and setup OpenGL buffers
    generateMesh();
    setupBuffers();
    
    // Enable depth testing and blending
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    return true;
}

void WaveRenderer::update(float deltaTime) {
    time += deltaTime;
}

void WaveRenderer::render(int screenWidth, int screenHeight) {
    // Set viewport
    glViewport(0, 0, screenWidth, screenHeight);
    
    // Clear with a sky color
    glClearColor(0.5f, 0.7f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Check for OpenGL errors
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error before rendering: " << error << std::endl;
    }
    
    shaderManager->use();
    
    // Setup matrices
    float aspect = (float)screenWidth / (float)screenHeight;
    
    // Simple perspective projection matrix
    float fov = 45.0f * M_PI / 180.0f;
    float near = 0.1f;
    float far = 100.0f;
    float f = 1.0f / tan(fov / 2.0f);
    
    float projection[16] = {
        f / aspect, 0, 0, 0,
        0, f, 0, 0,
        0, 0, (far + near) / (near - far), -1,
        0, 0, (2 * far * near) / (near - far), 0
    };
    
    // Simple view matrix (camera looking down at the water)
    float camX = sin(time * 0.1f) * 3.0f;
    float camY = 2.0f;
    float camZ = cos(time * 0.1f) * 3.0f;
    
    // Simplified view matrix calculation
    float view[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        -camX, -camY, -camZ, 1
    };
    
    // Set uniforms
    shaderManager->setMat4("projection", projection);
    shaderManager->setMat4("view", view);
    shaderManager->setFloat("time", time);
    shaderManager->setVec2("mousePos", (mouseX / screenWidth) * 2.0f - 1.0f, 
                                       (1.0f - mouseY / screenHeight) * 2.0f - 1.0f);
    shaderManager->setFloat("mousePressed", mousePressed ? 1.0f : 0.0f);
    shaderManager->setFloat("waveHeight", waveHeight);
    shaderManager->setFloat("waveFrequency", waveFrequency);
    shaderManager->setFloat("waveSpeed", waveSpeed);
    
    // Lighting
    shaderManager->setVec3("lightPos", 2.0f, 5.0f, 2.0f);
    shaderManager->setVec3("viewPos", camX, camY, camZ);
    
    // Render the wave mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}