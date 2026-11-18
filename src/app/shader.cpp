#include <glad/glad.h>

#include <array>
#include <fstream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <tonyMesh/app/shader.hpp>

namespace tonyMesh::app {

namespace {
constexpr std::array<std::string, 3> dirs = {"./shaders/", "../shaders/",
                                             "../../shaders/"};
std::optional<std::ifstream> findFile(const std::string& filePath) {
    for (const auto& path : dirs) {
        std::ifstream file(path + filePath);
        if (file) return file;
    }
    return std::nullopt;
}
}  // namespace

void Shader::use() const { glUseProgram(m_ID); }
Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCode{}, fragmentCode{};
    std::ifstream vShaderFile{}, fShaderFile{};

    // vShaderFile.exceptions();
    // fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    auto optVertex{findFile(vertexPath)}, optFragment(findFile(fragmentPath));
    if (!optVertex) {
        throw std::runtime_error("cannot load vertex shader " + vertexPath);
    }
    if (!optFragment) {
        throw std::runtime_error("cannot load fragment shader " + fragmentPath);
    }

    optVertex->exceptions(std::ifstream::failbit | std::ifstream::badbit);
    optFragment->exceptions(std::ifstream::failbit | std::ifstream::badbit);

    std::stringstream vShaderStream, fShaderStream;
    vShaderStream << optVertex->rdbuf();
    fShaderStream << optFragment->rdbuf();

    optVertex->close();
    optFragment->close();

    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();

    const char* vShaderCode{vertexCode.c_str()};
    const char* fShaderCode{fragmentCode.c_str()};

    unsigned vertex, fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    m_checkCompilation(vertex);

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    m_checkCompilation(fragment);

    m_ID = glCreateProgram();
    glAttachShader(m_ID, vertex);
    glAttachShader(m_ID, fragment);
    glLinkProgram(m_ID);
    m_checkCompilation(m_ID, true);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::m_checkCompilation(unsigned part, bool isProgram) {
    int success;
    char infoLog[512];

    if (isProgram) {
        glGetProgramiv(part, GL_LINK_STATUS, &success);
        if (!success) glGetProgramInfoLog(part, 512, nullptr, infoLog);
    } else {
        glGetProgramiv(part, GL_COMPILE_STATUS, &success);
        if (!success) glGetShaderInfoLog(part, 512, nullptr, infoLog);
    }

    if (!success)
        throw std::runtime_error("ERROR SHADER LOADING" + std::string(infoLog));
}
}  // namespace tonyMesh::app
