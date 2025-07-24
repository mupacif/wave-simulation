#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;
uniform float time;
uniform vec2 mousePos;
uniform float mousePressed;
uniform float waveHeight;
uniform float waveFrequency;
uniform float waveSpeed;

out vec3 FragPos;
out vec3 Normal;
out float Height;

void main() {
    vec3 pos = aPos;
    
    // Base wave pattern
    float wave1 = sin(pos.x * waveFrequency + time * waveSpeed) * cos(pos.z * waveFrequency + time * waveSpeed * 0.8);
    float wave2 = sin(pos.x * waveFrequency * 1.7 + time * waveSpeed * 1.3) * sin(pos.z * waveFrequency * 1.3 + time * waveSpeed);
    
    // Mouse interaction wave
    float mouseDist = distance(pos.xz, mousePos);
    float mouseWave = 0.0;
    if (mousePressed > 0.5) {
        mouseWave = sin(mouseDist * 10.0 - time * 8.0) * exp(-mouseDist * 2.0) * 0.5;
    }
    
    // Combine waves
    pos.y = (wave1 * 0.5 + wave2 * 0.3 + mouseWave) * waveHeight;
    
    // Calculate normal (approximate)
    float delta = 0.1;
    vec3 neighborX = vec3(pos.x + delta, pos.y, pos.z);
    vec3 neighborZ = vec3(pos.x, pos.y, pos.z + delta);
    
    // Recalculate heights for neighbors
    float waveX1 = sin(neighborX.x * waveFrequency + time * waveSpeed) * cos(neighborX.z * waveFrequency + time * waveSpeed * 0.8);
    float waveX2 = sin(neighborX.x * waveFrequency * 1.7 + time * waveSpeed * 1.3) * sin(neighborX.z * waveFrequency * 1.3 + time * waveSpeed);
    neighborX.y = (waveX1 * 0.5 + waveX2 * 0.3) * waveHeight;
    
    float waveZ1 = sin(neighborZ.x * waveFrequency + time * waveSpeed) * cos(neighborZ.z * waveFrequency + time * waveSpeed * 0.8);
    float waveZ2 = sin(neighborZ.x * waveFrequency * 1.7 + time * waveSpeed * 1.3) * sin(neighborZ.z * waveFrequency * 1.3 + time * waveSpeed);
    neighborZ.y = (waveZ1 * 0.5 + waveZ2 * 0.3) * waveHeight;
    
    vec3 tangentX = normalize(neighborX - pos);
    vec3 tangentZ = normalize(neighborZ - pos);
    Normal = normalize(cross(tangentZ, tangentX));
    
    FragPos = pos;
    Height = pos.y;
    
    gl_Position = projection * view * vec4(pos, 1.0);
}