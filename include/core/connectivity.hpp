#pragma once
#include <tonyMesh/common.hpp>
#include <vector>

namespace tonyMesh::connectivity {
    std::vector<Index> vertex_vertex(Index vert, const FaceMatrix& faces);
    std::vector<std::vector<Index>> vertex_vertex_lists(size_t numVertices, const FaceMatrix& faces);

    std::vector<Index> vertex_face(Index vert, const FaceMatrix& faces);
    std::vector<std::vector<Index>> vertex_faces_lists(Index numVertices, const FaceMatrix& faces);
}