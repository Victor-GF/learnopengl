//
// Created by victor on 4/19/26.
//

#pragma once

#include <string_view>
#include <glm/glm.hpp>

class Shader {
    unsigned int ID;

public:
    Shader() = default;
    Shader(std::string_view vertexShaderPath, std::string_view fragmentShaderPath);
    ~Shader();

    [[nodiscard]] constexpr unsigned int GetId() const { return ID; }

    void Use() const;
    void SetBool(std::string_view name, bool value) const;
    void SetInt(std::string_view name, int value) const;
    void SetFloat(std::string_view name, float value) const;
    void SetMat4(std::string_view name, glm::mat4 &value) const;
};
