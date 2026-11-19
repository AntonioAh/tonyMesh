#include <glad/glad.h>

#include <array>
#include <fstream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <tonyMesh/app/shader.hpp>
#include <utility>

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

// Compiles one stage. On failure the GL shader object is deleted before throwing.
unsigned compileShader(GLenum type, const char* source) {
    const unsigned shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success{};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024]{};
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        glDeleteShader(shader);
        throw std::runtime_error(std::string("shader compilation failed: ") +
                                 infoLog);
    }
    return shader;
}

void checkLink(unsigned program) {
    int success{};
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024]{};
        glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
        throw std::runtime_error(std::string("shader linking failed: ") +
                                 infoLog);
    }
}
}  // namespace

Shader::~Shader() {
    if (m_ID != 0) glDeleteProgram(m_ID);
}

Shader::Shader(Shader&& other) noexcept : m_ID(std::exchange(other.m_ID, 0)) {}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        if (m_ID != 0) glDeleteProgram(m_ID);
        m_ID = std::exchange(other.m_ID, 0);
    }
    return *this;
}

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

    // If anything throws, the destructor will not run (the object is not fully
    // constructed), so release every GL object created so far here.
    // glDeleteShader/glDeleteProgram silently ignore the name 0.
    unsigned vertex{}, fragment{};
    try {
        vertex = compileShader(GL_VERTEX_SHADER, vShaderCode);
        fragment = compileShader(GL_FRAGMENT_SHADER, fShaderCode);

        m_ID = glCreateProgram();
        glAttachShader(m_ID, vertex);
        glAttachShader(m_ID, fragment);
        glLinkProgram(m_ID);
        checkLink(m_ID);
    } catch (...) {
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteProgram(m_ID);
        m_ID = 0;
        throw;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}
}  // namespace tonyMesh::app
