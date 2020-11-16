#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "curves_math.hpp"
#include "math_help.hpp"

// do not use nullptr in OpenGL functions
// #define nullptr NULL

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

std::vector<char> shader_source_code(const std::string &filename);

GLuint createShader(GLuint shaderType, const std::string &file);

GLuint attachAndLinkShaders(GLuint vertexShader, GLuint fragmentShader);


std::pair<std::vector<float>, std::vector<unsigned>> customFigure(const std::vector<crv::point3> &keyPoints,
                                                                  size_t precision);

std::vector<crv::point3> cardioid(float a, size_t precision);

std::pair<std::vector<float>, std::vector<unsigned>> cubeFigure();

const char* glsl_version = "#version 450";

const GLuint WIDTH = 1000, HEIGHT = 800;
int figurePrecision = 2;
bool recalculateFigure = false;

float FOV = 45.0f;
float scaleX = 2.0f;
float scaleY = 2.0f;
float scaleZ = 2.0f;
float scaleSpeed = 1.0f;

float RotateX = 0.0f;
float RotateY = 0.0f;
float RotateZ = 0.0f;
glm::vec3 cameraPos = glm::vec3(4.0f, 0.0f, 4.0f);
glm::vec3 cameraFront = glm::vec3(-1.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 lightPos(2.0f, 0.0f, 0.5f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

float ambientStrength = 0.3;
float diffuseStrength = 0.7;
float specularStrength = 0.9;
int specularPow = 32;

int main() {


//    std::vector<float> v = fig.first;
//    std::vector<unsigned> idx = fig.second;
//
//    for (size_t i = 0; i < v.size(); i += 9) {
//        for (size_t j = i; j < i+9; j++) {
//            std::cout << v[j] << " ";
//        }
//        std::cout << '\n';
//    }
//
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Made by @devepodete. All rights reserved.", NULL, NULL);
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

    std::vector<crv::point3> keyPoints;

    auto tempTriangles = customFigure({{0, 0, 0}, {3, 3, -2}, {3, -3, 2}}, 40);
    std::vector<float> vertices = tempTriangles.first;
    std::cout << "vertices: " << vertices.size() << std::endl;
//    for (size_t i = 0; i < vertices.size(); i += 9) {
//        std::cout << vertices[i] << "," << vertices[i+1]<< "," << vertices[i+2] << '\n';
//    }
    std::vector<unsigned> indices = tempTriangles.second;

    auto tempCubeTriangles = cubeFigure();
    std::vector<float> cubeVertices = tempCubeTriangles.first;
    std::vector<unsigned> cubeIndices = tempCubeTriangles.second;

    glViewport(0, 0, WIDTH, HEIGHT);

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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *) (6 * sizeof(float)));
    //glEnableVertexAttribArray(2);


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
    //glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glm::mat4 projection = glm::perspective(glm::radians(FOV), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);


    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("../imgui/fonts/ProggyClean.ttf", 15.0f);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImGui::StyleColorsLight();
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        float time = (float) glfwGetTime();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClearColor(0.1f, 0.0f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO1);
        glUseProgram(shaderProgram);

        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

        glUniform1f(glGetUniformLocation(shaderProgram, "time"), time);
        glUniform1f(glGetUniformLocation(shaderProgram, "ambientStrength"), ambientStrength);
        glUniform1f(glGetUniformLocation(shaderProgram, "diffuseStrength"), diffuseStrength);
        glUniform1f(glGetUniformLocation(shaderProgram, "specularStrength"), specularStrength);
        glUniform1i(glGetUniformLocation(shaderProgram, "specularPow"), specularPow);


        if (recalculateFigure) {
            //tempTriangles = customFigure(keyPoints, figurePrecision);
            vertices = tempTriangles.first;
            indices = tempTriangles.second;
            std::cout << "1 vertices: " << vertices.size() << '\n';
            std::cout << "1 indices: " << indices.size() << '\n';
            glNamedBufferData(VBO1, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
            glNamedBufferData(EBO1, indices.size() * sizeof(unsigned), indices.data(), GL_DYNAMIC_DRAW);
            recalculateFigure = false;
        }

        //scaleX = 2*std::abs(std::sin((float)glfwGetTime() * scaleSpeed));
        //scaleY = std::abs(std::cos((float)glfwGetTime() * scaleSpeed));
        //scaleZ = std::abs(std::cos(time * scaleSpeed / 6)) + 0.5f;

        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::rotate(model, (float) glfwGetTime() / 2.0f, glm::vec3(0.0, 0.0, 1.0));
        model = glm::scale(model, glm::vec3(scaleX, scaleY, scaleZ));

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
        model = glm::scale(model, glm::vec3(0.1f, 0.1, 0.1));


        glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "projection"), 1, GL_FALSE,
                           glm::value_ptr(projection));
        lightColor = glm::vec3(clear_color.x, clear_color.y, clear_color.z);
        glUniform3f(glGetUniformLocation(lightShaderProgram, "lightColor"), lightColor.x, lightColor.y, lightColor.z);

        glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, 0);

        ImGui::Begin("Title");
        ImGui::Text("hello");
        ImGui::SliderFloat("rotation", &RotateX, 0.0f, 360.0f);
        ImGui::ColorEdit3("clear color", (float*)&clear_color);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO1);
    glDeleteBuffers(1, &VBO1);
    glDeleteBuffers(1, &EBO1);
    glDeleteProgram(shaderProgram);

    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &lightVBO);
    glDeleteBuffers(1, &lightEBO);
    glDeleteProgram(lightShaderProgram);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


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
        ambientStrength = std::max(std::min(ambientStrength - 0.1f, 1.0f), 0.0f);
    } else if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) {
        ambientStrength = std::max(std::min(ambientStrength + 0.1f, 1.0f), 0.0f);
    } else if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS) {
        diffuseStrength = std::max(std::min(diffuseStrength - 0.1f, 1.0f), 0.0f);
    } else if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS) {
        diffuseStrength = std::max(std::min(diffuseStrength + 0.1f, 1.0f), 0.0f);
    } else if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS) {
        specularStrength = std::max(std::min(specularStrength - 0.1f, 1.0f), 0.0f);
    } else if (glfwGetKey(window, GLFW_KEY_F6) == GLFW_PRESS) {
        specularStrength = std::max(std::min(specularStrength + 0.1f, 1.0f), 0.0f);
    } else if (glfwGetKey(window, GLFW_KEY_F7) == GLFW_PRESS) {
        specularPow = std::max(std::min(specularPow - 1, 32), 1);
    } else if (glfwGetKey(window, GLFW_KEY_F8) == GLFW_PRESS) {
        specularPow = std::max(std::min(specularPow + 1, 32), 1);
    }

    //std::cout << "amb: " << ambientStrength << ", diff: " << diffuseStrength << ", spec: " << specularStrength <<
    //          " | " << specularPow << '\n';
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

std::pair<std::vector<float>, std::vector<unsigned>> customFigure(const std::vector<crv::point3> &keyPoints,
                                                                  size_t precision) {
    float r = 1.0;
    float g = 1.0;
    float b = 1.0;
    std::vector<float> triangles;

    crv::Bezier bezier;
    bezier.setPrecision(precision);
    bezier.setKeyPoints(keyPoints);
    bezier.calculateCurve();
    std::cout << "bezier: " << bezier.points.size() << '\n';

    std::vector<crv::point3> cardi = cardioid(2.0, precision);
    std::cout << "Cardi: " << cardi.size() << '\n';

    std::vector<crv::point3> prev = cardi;
    for (crv::point3 &p : prev) {
        p += bezier.points[0];
    }
    for (size_t i = 1; i < bezier.points.size(); i++) {
        //std::cout << "i: " << i << '\n';
        std::vector<crv::point3> next = cardi;
        for (crv::point3 &p : next) {
            p += bezier.points[i];
        }

        for (size_t j = 0; j < prev.size(); j++) {
            size_t jnext = (j == prev.size()-1 ? 0 : j+1);
            glm::vec3 n1 = glm::cross(
                    (next[j]-prev[j]).asVec3(),
                    (next[jnext]-next[j]).asVec3()
                    );
            glm::vec3 n2 = glm::cross(
                    (prev[jnext]-next[jnext]).asVec3(),
                    (prev[j]-prev[jnext]).asVec3()
            );

            triangles.insert(triangles.end(), {
                prev[j].x, prev[j].y, prev[j].z, n1.x, n1.y, n1.z,// r, g, b,
                next[j].x, next[j].y, next[j].z, n1.x, n1.y, n1.z,// r, g, b,
                next[jnext].x, next[jnext].y, next[jnext].z, n1.x, n1.y, n1.z,// r, g, b,

                next[jnext].x, next[jnext].y, next[jnext].z, n2.x, n2.y, n2.z,// r, g, b,
                prev[jnext].x, prev[jnext].y, prev[jnext].z, n2.x, n2.y, n2.z,// r, g, b,
                prev[j].x, prev[j].y, prev[j].z, n2.x, n2.y, n2.z,// r, g, b,
            });
        }

        prev = next;
    }

    float fmin, fmax;
    fmin = 1e10;
    fmax = -1e10;
    for (size_t i = 0; i < triangles.size(); i += 6) {
        fmax = std::max(fmax, triangles[i]);
        fmax = std::max(fmax, triangles[i+1]);
        fmax = std::max(fmax, triangles[i+2]);
        fmin = std::min(fmin, triangles[i]);
        fmin = std::min(fmin, triangles[i+1]);
        fmin = std::min(fmin, triangles[i+2]);
    }

    std::cout << "fmin: " << fmin << " , fmax: " << fmax << '\n';
    float coeff = std::max(std::abs(fmin), std::abs(fmax));

    for (size_t i = 0; i < triangles.size(); i += 6) {
        triangles[i] /= coeff;
        triangles[i+1] /= coeff;
        triangles[i+2] /= coeff;
        triangles[i+3] /= coeff;
        triangles[i+4] /= coeff;
        triangles[i+5] /= coeff;
    }

    std::vector<unsigned> indices(triangles.size() / 6);
    for (size_t i = 0; i < indices.size(); i++) {
        indices[i] = i;
    }

    return {triangles, indices};
}

std::vector<crv::point3> cardioid(float a, size_t precision) {
    std::vector<crv::point3> res(precision);

    std::vector<float> phi = math::linspace(0, 2.0f * math::pi, precision);
    for(int i = 0; i < phi.size(); i++) {
        float r = 2.0f * a * (1.0f - std::cos(phi[i]));
        res[i] = {0.0f, r * std::sin(phi[i]), r * std::cos(phi[i])};
    }

    return res;
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
