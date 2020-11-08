#version 450 core

in vec4 ourColor;
out vec4 FragColor;

uniform float coeff;

void main() {
    FragColor = vec4(ourColor*coeff);
}