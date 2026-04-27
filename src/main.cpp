#include <array>
#include <cstdint>
#include <iostream>
#include <span>
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include "Shader.h"
#include "stb_image.h"

int window_width = 800;
int window_height = 600;
constexpr auto window_title = "LearnOpenGL";
GLFWwindow *g_Window;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = window_width / 2.0f;
float lastY = window_height / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow *window, const int width, const int height) {
    // window_width = width;
    // window_height = height;
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraMovement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraMovement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraMovement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraMovement::RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void triangle_coordinates(const Shader &shader) {
    auto view = glm::mat4(1.0f);
    auto projection = glm::mat4(1.0f);

    view = camera.GetViewMatrix();
    projection = glm::perspective(glm::radians(camera.Zoom),
                                  static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);

    shader.SetMat4("projection", projection);
    shader.SetMat4("view", view);
}

void update_window(const std::array<unsigned int, 1> &VAOs, const Shader &shader,
                   const std::array<unsigned int, 2> &textures, const std::array<glm::vec3, 10> &cubePositions,
                   const std::array<float, 180> &vertices) {
    processInput(g_Window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // Triangle
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    shader.Use();
    // const auto timeValue = static_cast<float>(glfwGetTime());
    // const float greenValue = (std::sin(timeValue) / 2.0f) + 0.5f;
    // const int vertexColorLocation = glGetUniformLocation(shaderProgram, "uniform_Color");
    // glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

    triangle_coordinates(shader);

    for (const unsigned int VAO: VAOs) {
        glBindVertexArray(VAO);

        uint16_t i = 0;
        for (const auto cubePosition: cubePositions) {
            auto model = glm::mat4(1.0f);
            model = glm::translate(model, cubePosition);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.SetMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);

            ++i;
        }
    }

    glfwSwapBuffers(g_Window);
    glfwPollEvents();
}

void hello_cubes(std::array<unsigned int, 1> &VAOs, std::array<unsigned int, 1> &VBOs, unsigned int &EBO,
                 std::unique_ptr<Shader> &shader, std::array<unsigned int, 2> &textures,
                 std::array<glm::vec3, 10> &cubePositions, std::array<float, 180> &vertices) {
    // Shaders
    const std::string resDir = "../res/";
    const std::string vertexShaderSrc = resDir + "/shader/shader.vs";
    const std::string fragmentShaderSrc = resDir + "/shader/shader.fs";
    shader = std::make_unique<Shader>(vertexShaderSrc, fragmentShaderSrc);

    glEnable(GL_DEPTH_TEST);

    vertices = {-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
                0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

                -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
                0.5f,  0.5f,  0.5f,  1.0f, 1.0f, -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

                -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
                -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

                0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
                0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

                -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
                0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

                -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f};

    cubePositions = {glm::vec3(0.0f, 0.0f, 0.0f),     glm::vec3(2.0f, 5.0f, -15.0f), glm::vec3(-1.5f, -2.2f, -2.5f),
                     glm::vec3(-3.8f, -2.0f, -12.3f), glm::vec3(2.4f, -0.4f, -3.5f), glm::vec3(-1.7f, 3.0f, -7.5f),
                     glm::vec3(1.3f, -2.0f, -2.5f),   glm::vec3(1.5f, 2.0f, -2.5f),  glm::vec3(1.5f, 0.2f, -1.5f),
                     glm::vec3(-1.3f, 1.0f, -1.5f)};

    // VBO, VAO and EBO
    glGenVertexArrays(static_cast<int>(VAOs.size()), VAOs.data());
    glGenBuffers(static_cast<int>(VBOs.size()), VBOs.data());
    // glGenBuffers(1, &EBO);

    for (int i = 0; i < VAOs.size(); ++i) {

        glBindVertexArray(VAOs[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        // position attrib
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(0));
        glEnableVertexAttribArray(0);
        // color atrrib
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // Texture attrib
        // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(6 *
        // sizeof(float))); glEnableVertexAttribArray(2);
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
    unsigned char *textureData = stbi_load(textureImagePath.c_str(), &width, &height, &nrChannels, 0);
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
    glfwSetCursorPosCallback(g_Window, mouse_callback);
    glfwSetScrollCallback(g_Window, scroll_callback);

    glfwSetInputMode(g_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    std::unique_ptr<Shader> shader;
    unsigned int VAO, VBO, EBO;
    std::array<unsigned int, 1> VAOs{}, VBOs{};
    std::array<unsigned int, 2> textures{};

    std::array<glm::vec3, 10> cubePositions{};
    std::array<float, 180> vertices{};

    hello_cubes(VAOs, VBOs, EBO, shader, textures, cubePositions, vertices);

    while (!glfwWindowShouldClose(g_Window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        update_window(VAOs, *shader, textures, cubePositions, vertices);
    }

    glDeleteVertexArrays(VAOs.size(), VAOs.data());
    glDeleteBuffers(VBOs.size(), VBOs.data());
    // glDeleteBuffers(1, &EBO);
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
