#include <array>
#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <memory>

#include "Shader.h"
#include "stb_image.h"

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

void update_window(const std::array<unsigned int, 1> &VAOs, const Shader &shader) {
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
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }

    glfwSwapBuffers(g_Window);
    glfwPollEvents();
}

void hello_triangle(std::array<unsigned int, 1> &VAOs, std::array<unsigned int, 1> &VBOs, unsigned int &EBO,
                    std::unique_ptr<Shader> &shader, unsigned int &texture) {
    const std::string resDir = "../res/";
    const std::string vertexShaderSrc = resDir + "/shader/shader.vs";
    const std::string fragmentShaderSrc = resDir + "/shader/shader.fs";
    shader = std::make_unique<Shader>(vertexShaderSrc, fragmentShaderSrc);

    std::array<std::array<float, 36>, 1> triangles{};
    triangles[0] = {
        // positions         // colors           // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
       -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
       -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    // VBO, VAO and EBO
    glGenVertexArrays(static_cast<int>(VAOs.size()), VAOs.data());
    glGenBuffers(static_cast<int>(VBOs.size()), VBOs.data());
    glGenBuffers(1, &EBO);

    for (int i = 0; i < triangles.size(); ++i) {
        auto &triangle = triangles[i];

        glBindVertexArray(VAOs[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        // position attrib
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(0));
        glEnableVertexAttribArray(0);
        // color atrrib
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // Texture attrib
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }

    // Texture
    int width, height, nrChannels;
    const std::string textureImagePath = resDir + "/textures/container.jpg";
    unsigned char* textureData = stbi_load(textureImagePath.c_str(), &width, &height, &nrChannels, 0);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (textureData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(textureData);

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
    unsigned int VAO, VBO, EBO, texture;
    std::array<unsigned int, 1> VAOs{}, VBOs{};

    hello_triangle(VAOs, VBOs, EBO, shader, texture);

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
