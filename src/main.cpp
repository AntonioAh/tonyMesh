#include <iostream>
#include <omp.h>
#include <Eigen/Dense>
#include <definitions.hpp>
#include <mesh.hpp>

int main() {
    #pragma omp parallel
    {
        #pragma omp single
        std::cout << "OpenMP attivo con " << omp_get_num_threads() << " thread." << std::endl;
    }

    Eigen::Matrix2d m;
    m << 1, 2, 3, 4;
    std::cout << "Eigen Matrix:\n" << m << std::endl;

    // Test dei costruttori di Mesh
    tonyMesh::VertexMatrix V(3, 3);
    V << 0.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 0.0f,
         0.0f, 1.0f, 0.0f;

    tonyMesh::FaceMatrix F(1, 3);
    F << 0, 1, 2;

    // 1. Default constructor
    tonyMesh::Mesh m_default;

    // 2. Parametrized constructor
    tonyMesh::Mesh m_param(V, F);

    // 3. Copy constructor
    tonyMesh::Mesh m_copy = m_param;

    // 4. Move constructor
    tonyMesh::Mesh m_move = std::move(m_copy);

    // 5. Copy assignment
    m_default = m_param;

    // 6. Move assignment
    m_default = std::move(m_move);

    std::cout << "Tutti i costruttori di Mesh funzionano correttamente!" << std::endl;

    // Test di Getter e Setter
    tonyMesh::Mesh mesh;
    mesh.setVertices(V);
    mesh.setFaces(F);

    std::cout << "Mesh creata con successo:" << std::endl;
    std::cout << " - Numero vertici: " << mesh.numVertices() << std::endl;
    std::cout << " - Numero facce: " << mesh.numFaces() << std::endl;
    std::cout << " - Vertici:\n" << mesh.getVertices() << std::endl;
    std::cout << " - Facce:\n" << mesh.getFaces() << std::endl;

    tonyMesh::VertexMatrix quadV(4, 3);
    quadV << 0.0f, 0.0f, 0.0f,
             1.0f, 0.0f, 0.0f,
             1.0f, 1.0f, 0.0f,
             0.0f, 1.0f, 0.0f;

    tonyMesh::FaceMatrix quadF(2, 3);
    quadF << 0, 1, 2,
             0, 2, 3;

    tonyMesh::Mesh quadMesh(quadV, quadF);

    auto vv = quadMesh.vertexVertexAdjacency();
    auto vf = quadMesh.vertexFaceAdjacency();
    auto ff = quadMesh.faceFaceAdjacency();
    auto edges = quadMesh.edges();
    auto bEdges = quadMesh.boundaryEdges();
    auto bVerts = quadMesh.boundaryVertices();

    std::cout << "Test Connettivita:" << std::endl;
    std::cout << " - Totale spigoli unici: " << edges.size() << std::endl;
    std::cout << " - Spigoli di bordo: " << bEdges.size() << std::endl;
    std::cout << " - Vertici di bordo: " << bVerts.size() << std::endl;
    std::cout << " - Mesh chiusa: " << (quadMesh.isClosed() ? "si" : "no") << std::endl;
    std::cout << " - Vicini vertice 0: ";
    for (auto v : quadMesh.vertexVertexNeighbors(0)) std::cout << v << " ";
    std::cout << std::endl;
    std::cout << " - Facce adiacenti a faccia 0: ";
    for (auto f : quadMesh.faceFaceNeighbors(0)) std::cout << f << " ";
    std::cout << std::endl;

    return 0;
}
