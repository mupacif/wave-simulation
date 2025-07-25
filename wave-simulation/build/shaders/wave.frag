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
    vec3 deepColor = vec3(0.1, 0.3, 0.6);
    vec3 shallowColor = vec3(0.2, 0.6, 0.9);
    vec3 foamColor = vec3(0.9, 0.95, 1.0);
    
    // Mix colors based on height (normalize height to reasonable range)
    float heightFactor = clamp((Height + 0.5) * 1.0, 0.0, 1.0);
    vec3 waterColor = mix(deepColor, shallowColor, heightFactor);
    
    // Add foam on wave peaks - adjusted for actual height ranges
    if (Height > 0.1) {
        float foamFactor = smoothstep(0.1, 0.3, Height);
        waterColor = mix(waterColor, foamColor, foamFactor * 0.5);
    }
    
    // Lighting
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normalize(Normal));
    
    // Diffuse - ensure it's always positive
    float diff = max(dot(normalize(Normal), lightDir), 0.2); // minimum ambient
    vec3 diffuse = diff * vec3(1.0);
    
    // Specular
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * vec3(1.0) * 0.3;
    
    // Fresnel effect
    float fresnel = pow(1.0 - max(dot(viewDir, normalize(Normal)), 0.0), 2.0);
    waterColor = mix(waterColor, vec3(0.8, 0.9, 1.0), fresnel * 0.3);
    
    // Combine - ensure minimum brightness
    vec3 result = waterColor * 0.6 + diffuse * waterColor * 0.4 + specular;
    result = max(result, waterColor * 0.3); // ensure minimum brightness
    
    // Add transparency
    float alpha = 0.9 + fresnel * 0.1;
    
    FragColor = vec4(result, alpha);
}