#pragma once
#include <string>

namespace tonyMesh::app {
class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    // Owns a GL program: copying would delete it twice. Moving transfers it.
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    void use() const;

    unsigned ID() const { return m_ID; }

private:
    unsigned m_ID{};
};
}  // namespace tonyMesh::app
