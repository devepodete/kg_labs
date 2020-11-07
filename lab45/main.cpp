#include <iostream>
#include <fstream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// do not use nullptr in OpenGL functions
// #define nullptr NULL


void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

std::vector<char> shader_source_code(const std::string &filename);
GLuint createShader(GLuint shaderType, const std::string &file);
GLuint attachAndLinkShaders(GLuint vertexShader, GLuint fragmentShader);

const GLuint WIDTH = 800, HEIGHT = 600;


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

    float vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f,  0.5f, 0.0f
    };

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint vertexShader = createShader(GL_VERTEX_SHADER, "../shaders/shader.vert");
    GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, "../shaders/shader.frag");
    GLuint shaderProgram = attachAndLinkShaders(vertexShader, fragmentShader);
    glUseProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    std::cout << "--- SHADERS INITIALIZED ---\n";

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    


    glViewport(0, 0, WIDTH, HEIGHT);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}



void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    std::cout << key << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
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
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "failed to link shaders\n";
        std::cout << infoLog << std::endl;
        throw std::runtime_error("");
    }

    return shaderProgram;
}