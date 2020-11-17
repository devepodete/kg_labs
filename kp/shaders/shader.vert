#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
//layout (location = 2) in vec3 aColor;

out vec3 FragPos;
out vec3 Normal;

uniform float time;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    //objectColor = vec3(max(abs(tan(time * 2 * aPos.z)), 0.4), sin(aPos.z * time) + cos(time), min(abs(sin(time * aPos.z)), 0.5));
}