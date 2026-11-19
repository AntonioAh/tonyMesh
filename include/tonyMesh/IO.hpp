#pragma once

#include <optional>
#include <string>
#include <tonyMesh/common.hpp>
#include <tonyMesh/mesh.hpp>

namespace tonyMesh::IO {
std::optional<Mesh> readMesh(std::string_view filePath);
void writeMesh(const Mesh& mesh, std::string_view filePath);
}  // namespace tonyMesh::IO
