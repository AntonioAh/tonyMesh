#pragma once
#include <string>

namespace tonyMesh::app {
class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    void use() const;

    unsigned ID() const { return m_ID; }

private:
    unsigned m_ID;

    void m_checkCompilation(unsigned part, bool isProgram = false);
};
}  // namespace tonyMesh::app
