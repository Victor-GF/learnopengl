//
// Created by victor on 4/19/26.
//
#include "Shader.h"

#include <fstream>
#include <glad/glad.h>
#include <iostream>
#include <sstream>
#include <string>

Shader::Shader(const std::string_view vertexShaderPath, const std::string_view fragmentShaderPath) {
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // open files
        vShaderFile.open(vertexShaderPath.data());
        fShaderFile.open(fragmentShaderPath.data());
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure &) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    // 2. compile shaders
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    // print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    // similiar for Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    // print linking errors if any
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}
Shader::~Shader() { glDeleteProgram(ID); }

void Shader::Use() const { glUseProgram(ID); }

void Shader::SetBool(const std::string_view name, const bool value) const {
    if (const auto location = glGetUniformLocation(ID, name.data()); location == -1) {
        std::cout << "ERROR::SHADER::SET_BOOL::NAME_NOT_FOUND" << name << std::endl;
    } else {
        glUniform1i(location, static_cast<int>(value));
    }
}

void Shader::SetInt(const std::string_view name, const int value) const {
    if (const auto location = glGetUniformLocation(ID, name.data()); location == -1) {
        std::cout << "ERROR::SHADER::SET_INT::NAME_NOT_FOUND" << name << std::endl;
    } else {
        glUniform1i(location, value);
    }
}

void Shader::SetFloat(const std::string_view name, const float value) const {
    if (const auto location = glGetUniformLocation(ID, name.data()); location == -1) {
        std::cout << "ERROR::SHADER::SET_FLOAT::NAME_NOT_FOUND" << name << std::endl;
    } else {
        glUniform1f(location, value);
    }
}
