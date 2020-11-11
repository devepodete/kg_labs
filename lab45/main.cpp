#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "math_help.hpp"


// do not use nullptr in OpenGL functions
// #define nullptr NULL

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

std::vector<char> shader_source_code(const std::string &filename);

GLuint createShader(GLuint shaderType, const std::string &file);

GLuint attachAndLinkShaders(GLuint vertexShader, GLuint fragmentShader);


std::pair<std::vector<float>, std::vector<unsigned>> customFigure(size_t precision);
std::pair<std::vector<float>, std::vector<unsigned>> cubeFigure();

const GLuint WIDTH = 800, HEIGHT = 600;
int figurePrecision = 2;
bool recalculateFigure = false;

float FOV = 45.0f;
float RotateX = 0.0f;
float RotateY = 0.0f;
float RotateZ = 0.0f;
glm::vec3 cameraPos = glm::vec3(4.0f, 0.0f, 4.0f);
glm::vec3 cameraFront = glm::vec3(-1.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 lightPos(2.0f, 0.0f, 0.5f);

float ambientStrength = 0.3;
float diffuseStrength = 0.7;
float specularStrength = 0.9;
int specularPow = 32;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
    glfwMakeContextCurrent(window);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return EXIT_FAILURE;
    }

    auto tempTriangles = customFigure(figurePrecision);
    std::vector<float> vertices = tempTriangles.first;
    std::vector<unsigned> indices = tempTriangles.second;

    auto tempCubeTriangles = cubeFigure();
    std::vector<float> cubeVertices = tempCubeTriangles.first;
    std::vector<unsigned> cubeIndices = tempCubeTriangles.second;


    GLuint vertexShader = createShader(GL_VERTEX_SHADER, "../shaders/shader.vert");
    GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, "../shaders/shader.frag");
    GLuint shaderProgram = attachAndLinkShaders(vertexShader, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLuint lightVertexShader = createShader(GL_VERTEX_SHADER, "../shaders/lightShader.vert");
    GLuint lightFragmentShader = createShader(GL_FRAGMENT_SHADER, "../shaders/lightShader.frag");
    GLuint lightShaderProgram = attachAndLinkShaders(lightVertexShader, lightFragmentShader);
    glDeleteShader(lightVertexShader);
    glDeleteShader(lightFragmentShader);
    std::cout << "--- SHADERS INITIALIZED ---\n";


    GLuint VAO1, VBO1, EBO1;
    glGenVertexArrays(1, &VAO1);
    glBindVertexArray(VAO1);
    glGenBuffers(1, &VBO1);
    glGenBuffers(1, &EBO1);
    

    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), indices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    
    GLuint lightVAO, lightVBO, lightEBO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glGenBuffers(1, &lightVBO);
    glGenBuffers(1, &lightEBO);

    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(float), cubeVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size() * sizeof(unsigned), cubeIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);


    glUseProgram(shaderProgram);

    glCullFace(GL_CW);
    glEnable(GL_CULL_FACE);
    //glPolygonMode(GL_CCW, GL_LINE);
    glEnable(GL_DEPTH_TEST);

    glm::mat4 projection = glm::perspective(glm::radians(FOV), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO1);
        glUseProgram(shaderProgram);

        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

        glUniform1f(glGetUniformLocation(shaderProgram, "ambientStrength"), ambientStrength);
        glUniform1f(glGetUniformLocation(shaderProgram, "diffuseStrength"), diffuseStrength);
        glUniform1f(glGetUniformLocation(shaderProgram, "specularStrength"), specularStrength);
        glUniform1i(glGetUniformLocation(shaderProgram, "specularPow"), specularPow);


        if (recalculateFigure) {
            tempTriangles = customFigure(figurePrecision);
            vertices = tempTriangles.first;
            indices = tempTriangles.second;
            glNamedBufferData(VBO1, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
            glNamedBufferData(EBO1, indices.size() * sizeof(unsigned), indices.data(), GL_DYNAMIC_DRAW);
            recalculateFigure = false;
        }

        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::rotate(model, (float) glfwGetTime() / 2.0f, glm::vec3(0.0, 0.0, 1.0));
        //model = glm::scale(model, glm::vec3(0.7, 0.5, 0.5));
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        view = glm::rotate(view, glm::radians(RotateX), glm::vec3(1.0, 0.0, 0.0));
        view = glm::rotate(view, glm::radians(RotateY), glm::vec3(0.0, 1.0, 0.0));
        view = glm::rotate(view, glm::radians(RotateZ), glm::vec3(0.0, 0.0, 1.0));

        glm::vec3 tempCameraPos = glm::vec3(glm::vec4(cameraPos, 1.0) * view);

        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), tempCameraPos.x, tempCameraPos.y, tempCameraPos.z);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);


        glBindVertexArray(lightVAO);
        glUseProgram(lightShaderProgram);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.1f));

        glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3f(glGetUniformLocation(lightShaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);

        glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO1);
    glDeleteBuffers(1, &VBO1);
    glDeleteBuffers(1, &EBO1);
    glDeleteProgram(shaderProgram);

    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &lightVBO);
    glDeleteBuffers(1, &lightEBO);
    glDeleteProgram(lightShaderProgram);

    glfwTerminate();

    return EXIT_SUCCESS;
}


void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    //std::cout << key << std::endl;
    float lightSpeed = 0.3f;
    float rotateSpeed = 3.0f;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);

    } else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        lightPos.x -= lightSpeed;
    } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        lightPos.x += lightSpeed;
    } else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        lightPos.y -= lightSpeed;
    } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        lightPos.y += lightSpeed;
    } else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        lightPos.z -= lightSpeed;
    } else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        lightPos.z += lightSpeed;

    } else if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        RotateZ += rotateSpeed;
    } else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        RotateZ -= rotateSpeed;
    } else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        RotateY += rotateSpeed;
    } else if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        RotateY -= rotateSpeed;
    } else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        RotateX += rotateSpeed;
    } else if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        RotateX -= rotateSpeed;

    } else if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
        figurePrecision++;
        recalculateFigure = true;
    } else if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
        figurePrecision--;
        recalculateFigure = true;
    } else if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
        ambientStrength = std::max(std::min(ambientStrength-0.1f, 1.0f), 0.0f);
    } else if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) {
        ambientStrength = std::max(std::min(ambientStrength+0.1f, 1.0f), 0.0f);
    } else if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS) {
        diffuseStrength = std::max(std::min(diffuseStrength-0.1f, 1.0f), 0.0f);
    } else if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS) {
        diffuseStrength = std::max(std::min(diffuseStrength+0.1f, 1.0f), 0.0f);
    } else if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS) {
        specularStrength = std::max(std::min(specularStrength-0.1f, 1.0f), 0.0f);
    } else if (glfwGetKey(window, GLFW_KEY_F6) == GLFW_PRESS) {
        specularStrength = std::max(std::min(specularStrength+0.1f, 1.0f), 0.0f);
    } else if (glfwGetKey(window, GLFW_KEY_F7) == GLFW_PRESS) {
        specularPow = std::max(std::min(specularPow-1, 32), 1);
    } else if (glfwGetKey(window, GLFW_KEY_F8) == GLFW_PRESS) {
        specularPow = std::max(std::min(specularPow+1, 32), 1);
    }

    std::cout << "amb: " << ambientStrength << ", diff: " << diffuseStrength << ", spec: " << specularStrength <<
    " | " << specularPow << '\n';
    if (figurePrecision < 2) {
        figurePrecision = 2;
    }
}


void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}


std::vector<char> shader_source_code(const std::string &filename) {
    std::ifstream file(filename, std::ios::in | std::ios::ate);

    if (!file.is_open()) {
        std::cout << "Failed to open file " << filename << '\n';
        std::cout << "If such an error occurs, check executable file path and\n";
        std::cout << "set shaders files path relatively to it" << std::endl;
        throw std::runtime_error("");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

GLuint createShader(GLuint shaderType, const std::string &file) {
    std::vector<char> shaderData = shader_source_code(file);
    shaderData.push_back('\0');
    const char *shaderSource = shaderData.data();

    GLuint shaderID;
    shaderID = glCreateShader(shaderType);
    glShaderSource(shaderID, 1, &shaderSource, NULL);
    glCompileShader(shaderID);

    int success;
    char infoLog[512];
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
        std::cout << infoLog << std::endl;
        throw std::runtime_error("Failed to compile shader " + file);
    }

    return shaderID;
}

GLuint attachAndLinkShaders(GLuint vertexShader, GLuint fragmentShader) {
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "failed to link shaders\n";
        std::cout << infoLog << std::endl;
        throw std::runtime_error("");
    }

    return shaderProgram;
}

std::pair<std::vector<float>, std::vector<unsigned>> customFigure(size_t precision) {
    float figureR = 0.7f;
    float figureG = 0.4f;
    float figureB = 0.3f;
    
    float sphereX = 0.0f;
    float sphereY = 0.0f;
    float sphereRadius = 1.0f;
    std::vector<float> radiuses = math::linspace(sphereRadius, 0.0f, precision);

    std::vector<std::vector<std::pair<float, float>>> circles(precision);

    float z = 0.0f;
    float zStep = sphereRadius / circles.size();

    for (size_t i = 0; i < precision; i++) {
        circles[i] = math::regularPolygon(precision + 1, std::sqrt(sphereRadius * sphereRadius - z * z), sphereX,
                                          sphereY);
        circles[i].push_back(circles[i][0]);
        //std::cout << std::sqrt(1.0f - z * z) << " ";
        z += zStep;
    }

    std::vector<float> triangles;
    z = 0.0f;
    zStep = 1.0f / circles.size();

    for (size_t i = 0; i < circles.size() - 1; i++) {
        for (size_t j = 0; j < circles[i].size() - 1; j++) {
            glm::vec3 v1(circles[i][j + 1].first-circles[i][j].first,
                         circles[i][j + 1].second-circles[i][j].second,
                         z-z);
            glm::vec3 v2(circles[i + 1][j].first-circles[i][j + 1].first,
                         circles[i + 1][j].second-circles[i][j + 1].second,
                         z + zStep - z);
            glm::vec3 n = glm::cross(v1, v2);
            //std::cout << n.x << " " << n.y << " " << n.z << std::endl;

            triangles.insert(triangles.end(), {
                    circles[i][j].first, circles[i][j].second, z, n.x, n.y, n.z, figureR, figureG, figureB,
                    circles[i][j + 1].first, circles[i][j + 1].second, z, n.x, n.y, n.z, figureR, figureG, figureB,
                    circles[i + 1][j].first, circles[i + 1][j].second, z + zStep, n.x, n.y, n.z, figureR, figureG, figureB,
            });

            v1 = glm::vec3(circles[i + 1][j + 1].first-circles[i][j + 1].first,
                           circles[i + 1][j + 1].second-circles[i][j + 1].second,
                           z + zStep - z);
            v2 = glm::vec3(circles[i + 1][j].first-circles[i + 1][j + 1].first,
                           circles[i + 1][j].second-circles[i + 1][j + 1].second,
                           z + zStep - (z + zStep));
            n = glm::cross(v1, v2);

            triangles.insert(triangles.end(), {
                    circles[i][j + 1].first, circles[i][j + 1].second, z, n.x, n.y, n.z, figureR, figureG, figureB,
                    circles[i + 1][j + 1].first, circles[i + 1][j + 1].second, z + zStep, n.x, n.y, n.z, figureR, figureG, figureB,
                    circles[i + 1][j].first, circles[i + 1][j].second, z + zStep, n.x, n.y, n.z, figureR, figureG, figureB,
            });


            if (i + 1 == circles.size() - 1) {
                v1 = glm::vec3(circles[i + 1][j + 1].first-circles[i + 1][j].first,
                               circles[i + 1][j + 1].second-circles[i + 1][j].second,
                               z + zStep - (z + zStep));
                v2 = glm::vec3(sphereX-circles[i + 1][j + 1].first,
                               sphereY-circles[i + 1][j + 1].second,
                               sphereRadius - (z + zStep));
                n = glm::cross(v1, v2);

                triangles.insert(triangles.end(), {
                        circles[i + 1][j].first, circles[i + 1][j].second, z + zStep, n.x, n.y, n.z, figureR, figureG, figureB,
                        circles[i + 1][j + 1].first, circles[i + 1][j + 1].second, z + zStep, n.x, n.y, n.z, figureR, figureG, figureB,
                        sphereX, sphereY, sphereRadius, n.x, n.y, n.z, figureR, figureG, figureB,
                });
            }
        }
        z += zStep;
    }


    std::vector<unsigned> indices(triangles.size() / 9);
    for (size_t i = 0; i < indices.size(); i++) {
        indices[i] = i;
    }

    return {triangles, indices};
}

std::pair<std::vector<float>, std::vector<unsigned>> cubeFigure() {

    std::vector<float> vertices = {
            1.0, 1.0, -1.0, //0
            -1.0, 1.0, -1.0,
            -1.0, -1.0, -1.0,
            1.0, -1.0, -1.0,

            1.0, 1.0, 1.0, //4
            -1.0, 1.0, 1.0,
            -1.0, -1.0, 1.0,
            1.0, -1.0, 1.0,
    };
    std::vector<unsigned> indices = {
            0, 1, 4,
            1, 5, 4,
            1, 2, 5,
            2, 6, 5,
            2, 3, 6,
            3, 7, 6,
            0, 7, 3,
            0, 4, 7,

            4, 5, 6,
            6, 7, 4,

            0, 2, 1,
            0, 3, 2,
    };


    return {vertices, indices};
}