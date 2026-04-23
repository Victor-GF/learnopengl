#include <array>
#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <memory>

#include "Shader.h"
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

void update_window(const std::array<unsigned int, 1> &VAOs, const Shader &shader,
                   const std::array<unsigned int, 2> &textures) {
    processInput(g_Window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // Triangle
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    shader.Use();
    const auto timeValue = static_cast<float>(glfwGetTime());
    // const float greenValue = (std::sin(timeValue) / 2.0f) + 0.5f;
    // const int vertexColorLocation = glGetUniformLocation(shaderProgram, "uniform_Color");
    // glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
    auto trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
    trans = glm::rotate(trans, std::sin(timeValue), glm::vec3(0.0f, 0.0f, 1.0f));
    const unsigned int transformLoc = glGetUniformLocation(shader.GetId(), "transform");
    glUniformMatrix4fv(static_cast<GLint>(transformLoc), 1, GL_FALSE, glm::value_ptr(trans));
    for (const unsigned int VAO: VAOs) {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }

    glfwSwapBuffers(g_Window);
    glfwPollEvents();
}

void hello_triangle(std::array<unsigned int, 1> &VAOs, std::array<unsigned int, 1> &VBOs, unsigned int &EBO,
                    std::unique_ptr<Shader> &shader, std::array<unsigned int, 2>&textures) {
    // Shaders
    const std::string resDir = "../res/";
    const std::string vertexShaderSrc = resDir + "/shader/shader.vs";
    const std::string fragmentShaderSrc = resDir + "/shader/shader.fs";
    shader = std::make_unique<Shader>(vertexShaderSrc, fragmentShaderSrc);

    std::array<std::array<float, 36>, 1> triangles{};
    triangles[0] = {
        // positions          // colors          // texture coords (note that we changed them to 'zoom in' on our texture image)
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
       -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
       -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
    };
    const unsigned int indices[] = {
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
    glGenTextures(static_cast<int>(textures.size()), textures.data());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    std::string textureImagePath = resDir + "/textures/container.jpg";
    unsigned char* textureData = stbi_load(textureImagePath.c_str(), &width, &height, &nrChannels, 0);
    if (textureData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture 1" << std::endl;
    }
    stbi_image_free(textureData);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    textureImagePath = resDir + "/textures/awesomeface.png";
    textureData = stbi_load(textureImagePath.c_str(), &width, &height, &nrChannels, 0);
    if (textureData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture 2" << std::endl;
    }
    stbi_image_free(textureData);

    shader->Use();
    shader->SetInt("uniformTexture1", 0);
    shader->SetInt("uniformTexture2", 1);
    shader->SetFloat("mixInterpolate", 0.1f);

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
    std::array<unsigned int, 1> VAOs{}, VBOs{};
    std::array<unsigned int, 2> textures{};

    hello_triangle(VAOs, VBOs, EBO, shader, textures);

    while (!glfwWindowShouldClose(g_Window)) {
        update_window(VAOs, *shader, textures);
    }

    glDeleteVertexArrays(VAOs.size(), VAOs.data());
    glDeleteBuffers(VBOs.size(), VBOs.data());
    glDeleteBuffers(1, &EBO);
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
