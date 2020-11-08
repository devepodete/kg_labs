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


const GLuint WIDTH = 800, HEIGHT = 600;
int figurePrecision = 2;
bool recalculateFigure = false;
float FOV = 45.0f;
glm::vec3 cameraPos = glm::vec3(2.0f, 0.0f, 2.0f);
glm::vec3 cameraFront = glm::vec3(-2.0f, 0.0f, -2.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, 1.0f);

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
    std::vector<unsigned int> indices = tempTriangles.second;




    GLuint vertexShader = createShader(GL_VERTEX_SHADER, "../shaders/shader.vert");
    GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, "../shaders/shader.frag");
    GLuint shaderProgram = attachAndLinkShaders(vertexShader, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    std::cout << "--- SHADERS INITIALIZED ---\n";


    GLuint VAO1, VBO1, EBO1;
    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);
    glGenBuffers(1, &EBO1);
    glBindVertexArray(VAO1);


    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glUseProgram(shaderProgram);

    glCullFace(GL_CW);
    glEnable(GL_CULL_FACE);
    //glPolygonMode(GL_CCW, GL_LINE);
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (recalculateFigure) {
            tempTriangles = customFigure(figurePrecision);
            vertices = tempTriangles.first;
            indices = tempTriangles.second;
            recalculateFigure = false;
        }

        glNamedBufferData(VBO1, vertices.size()*sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
        glNamedBufferData(EBO1, indices.size()*sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime()/2.0f, glm::vec3(0.0, 0.0, 1.0));
        //model = glm::scale(model, glm::vec3(0.7, 0.5, 0.5));
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(FOV), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        //glDrawArrays(GL_TRIANGLES, 0, indices.size());
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO1);
    glDeleteBuffers(1, &VBO1);
    glDeleteBuffers(1, &EBO1);
    glDeleteProgram(shaderProgram);

    glfwTerminate();

    return EXIT_SUCCESS;
}


void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    //std::cout << key << std::endl;
    const float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);

    } else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraPos += cameraSpeed * cameraFront;
    } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPos -= cameraSpeed * cameraFront;
    } else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    } else if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
        figurePrecision++;
        recalculateFigure = true;
    } else if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
        figurePrecision--;
        recalculateFigure = true;
    }

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

std::pair<std::vector<float>, std::vector<unsigned int>> customFigure(size_t precision) {
    std::vector<float> radiuses = math::linspace(1.0f, 0.0f, precision);

    std::vector<std::vector<std::pair<float, float>>> circles(precision);

    for (size_t i = 0; i < precision; i++) {
        circles[i] = math::regularPolygon(precision+1, std::sqrt(radiuses[i]), 0.0, 0.0);
        circles[i].push_back(circles[i][0]);
    }

    std::vector<float> triangles;
    float z = 0.0f;
    float zStep = 1.0f / circles.size();

    for (size_t i = 0; i < circles.size()-1; i++) {
        for (size_t j = 0; j < circles[i].size()-1; j++) {
            triangles.insert(triangles.end(), {
                    circles[i][j].first, circles[i][j].second, z, 1.0f, 1.0f, 1.0f,
                    circles[i][j+1].first, circles[i][j+1].second, z, 1.0f, 1.0f, 1.0f,
                    circles[i+1][j].first, circles[i+1][j].second, z + zStep, 1.0f, 1.0f, 1.0f,
            });

            if (i+1 == circles.size()-1) {
                continue;
            }

            triangles.insert(triangles.end(), {
                    circles[i][j + 1].first, circles[i][j + 1].second, z,  1.0f, 1.0f, 1.0f,
                    circles[i + 1][j + 1].first, circles[i + 1][j + 1].second, z + zStep,  1.0f, 1.0f, 1.0f,
                    circles[i + 1][j].first, circles[i + 1][j].second, z + zStep,  1.0f, 1.0f, 1.0f,
            });
        }
        z += zStep;
    }


    std::vector<unsigned int> indices(triangles.size()/6);
    for (size_t i = 0; i < indices.size(); i++) {
        indices[i] = i;
    }

    return {triangles, indices};
}