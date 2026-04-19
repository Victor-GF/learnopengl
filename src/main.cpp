#include <array>
#include <iostream>
#include <math.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <memory>

#include "Shader.h"

constexpr int window_width = 800;
constexpr int window_height = 600;
constexpr auto window_title = "LearnOpenGL";
GLFWwindow *g_Window;

void framebuffer_size_callback(GLFWwindow *window, const int width, const int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void update_window(const std::array<unsigned int, 2> &VAOs, Shader &shader) {
    processInput(g_Window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // Triangle
    shader.Use();
    // const auto timeValue = static_cast<float>(glfwGetTime());
    // const float greenValue = (std::sin(timeValue) / 2.0f) + 0.5f;
    // const int vertexColorLocation = glGetUniformLocation(shaderProgram, "uniform_Color");
    // glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
    for (const unsigned int VAO: VAOs) {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    glfwSwapBuffers(g_Window);
    glfwPollEvents();
}

void hello_triangle(std::array<unsigned int, 2> &VAOs, std::array<unsigned int, 2> &VBOs, unsigned int &EBO,
                    std::unique_ptr<Shader> &shader) {
    const std::string shaderDir = "../res/shader/";
    const std::string vertexShaderSrc = shaderDir + "shader.vs";
    const std::string fragmentShaderSrc = shaderDir + "shader.fs";
    shader = std::make_unique<Shader>(vertexShaderSrc, fragmentShaderSrc);

    std::array<std::array<float, 18>, 1> triangles{};
    triangles[0] = {
            // positions        // colors
            0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
            0.0f,  -0.5f,  0.0f, 0.0f, 0.0f, 1.0f // top
    };

    // VBO, VAO and EBO
    glGenVertexArrays(2, VAOs.data());
    glGenBuffers(2, VBOs.data());

    for (int i = 0; i < triangles.size(); ++i) {
        auto &triangle = triangles[i];

        glBindVertexArray(VAOs[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle.data(), GL_STATIC_DRAW);
        // position attrib
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(0));
        glEnableVertexAttribArray(0);
        // color atrrib
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}


void setup_window() {
    g_Window = glfwCreateWindow(window_width, window_height, window_title, nullptr, nullptr);
    if (g_Window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(g_Window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glfwSetFramebufferSizeCallback(g_Window, framebuffer_size_callback);

    std::unique_ptr<Shader> shader;
    unsigned int VAO, VBO, EBO;
    std::array<unsigned int, 2> VAOs{}, VBOs{};

    hello_triangle(VAOs, VBOs, EBO, shader);

    while (!glfwWindowShouldClose(g_Window)) {
        update_window(VAOs, *shader);
    }

    glDeleteVertexArrays(VAOs.size(), VAOs.data());
    glDeleteBuffers(VBOs.size(), VBOs.data());
    shader.reset();
    glfwTerminate();
}


int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    setup_window();

    return 0;
}
