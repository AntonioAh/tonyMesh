#include <mesh.hpp>
#include <omp.h>
#include <algorithm>
#include <utility>
#include <vector>

namespace tonyMesh {
    Mesh::Mesh(const VertexMatrix& vertices, const FaceMatrix& faces)
        : m_vertices(vertices), m_faces(faces) {
        activate();
    }

    Mesh::Mesh(VertexMatrix &&vertices, FaceMatrix &&faces) : m_vertices(std::move(vertices)), m_faces(std::move(faces)){
        activate();
    }

    void Mesh::activate() {
        m_minBounding = m_vertices.colwise().minCoeff();
        m_maxBounding = m_vertices.colwise().maxCoeff();
    }
}

