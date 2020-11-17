#version 450 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform int specularPow;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 objectColor;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    vec3 ambient = ambientStrength * lightColor;


    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor;

    float spec = 0.0;
    if (diff > 0) {
        spec = pow(max(dot(viewDir, reflectDir), 0.0), specularPow);
    }
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
