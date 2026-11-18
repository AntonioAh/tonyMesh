#pragma once

#include <string>
#include <tonyMesh/common.hpp>
#include <tonyMesh/mesh.hpp>

namespace tonyMesh::IO {
Mesh readMesh(const std::string& filePath);
void writeMesh(const Mesh& mesh, const std::string& filePath);
}  // namespace tonyMesh::IO
