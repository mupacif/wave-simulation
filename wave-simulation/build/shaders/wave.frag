#version 330 core

in vec3 FragPos;
in vec3 Normal;
in float Height;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float time;

void main() {
    // Water colors
    vec3 deepColor = vec3(0.0, 0.2, 0.4);
    vec3 shallowColor = vec3(0.0, 0.5, 0.8);
    vec3 foamColor = vec3(0.9, 0.95, 1.0);
    
    // Mix colors based on height
    float heightFactor = (Height + 1.0) * 0.5;
    vec3 waterColor = mix(deepColor, shallowColor, heightFactor);
    
    // Add foam on wave peaks
    if (Height > 0.3) {
        float foamFactor = smoothstep(0.3, 0.5, Height);
        waterColor = mix(waterColor, foamColor, foamFactor * 0.7);
    }
    
    // Lighting
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);
    
    // Diffuse
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0);
    
    // Specular
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * vec3(1.0) * 0.5;
    
    // Fresnel effect
    float fresnel = pow(1.0 - dot(viewDir, Normal), 2.0);
    waterColor = mix(waterColor, vec3(0.8, 0.9, 1.0), fresnel * 0.5);
    
    // Combine
    vec3 result = waterColor * 0.3 + diffuse * waterColor * 0.5 + specular;
    
    // Add transparency
    float alpha = 0.8 + fresnel * 0.2;
    
    FragColor = vec4(result, alpha);
}