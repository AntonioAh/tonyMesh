#include <core/smoothing.hpp>
#include <tbb/parallel_for.h>

#include "core/connectivity.hpp"

namespace tonyMesh::smoothing {

    static void createMidPoints(VertexMatrix& vertices, FaceMatrix& facesSecond, const Mesh& originalMesh){
        const auto& originalVertices{originalMesh.vertices()};
        const auto& originalFaces{originalMesh.faces()};

        tbb::parallel_for(Index{}, originalFaces.rows(), [&](Index i) {
            vertices.row(originalVertices.rows() + i) = ( originalVertices.row(originalFaces(i, 0)) +
                                originalVertices.row(originalFaces(i, 1)) +
                                originalVertices.row(originalFaces(i, 2))) / 3;

            facesSecond.row(3*i + 0) << originalFaces(i, 0), originalFaces(i, 1), originalVertices.rows()+i;
            facesSecond.row(3*i + 1) << originalFaces(i, 1), originalFaces(i, 2), originalVertices.rows()+i;
            facesSecond.row(3*i + 2) << originalFaces(i, 2), originalFaces(i, 0), originalVertices.rows()+i;
        });
    }

    static void movePoints(VertexMatrix& vertices, const Mesh& originalMesh){
        const auto boundaryVertices{originalMesh.isBoundaryVertex()};
        const auto& originalVertices{originalMesh.vertices()};

        tbb::parallel_for(Index{}, originalVertices.rows(), [&](Index v) {
            if (boundaryVertices[v]) {
                vertices.row(v) = originalVertices.row(v);
                return;
            }
            const auto neighbors{originalMesh.getNeighbors(v)};
            const Real n(neighbors.size());

            Eigen::RowVector<Real, 3> sum;
            sum.setZero();
            for (VertexIndex neigh : neighbors) {
                sum += originalVertices.row(neigh);
            }

            Real an{(4.0 - 2.0*std::cos(2*M_PI/n)) / 9.0};
            vertices.row(v) = (1 - an) * originalVertices.row(v) + an / n * sum;
        });
    }

    static void flipEdges(FaceMatrix& faces, const Mesh& mesh) {
        const auto nOrVertices{mesh.numVertices()};

        auto edges = tonyMesh::connectivity::edgeTopology(faces);

        tbb::parallel_for(size_t{}, edges.size(), [&](size_t i){
            auto& [edge, adj] = edges[i];
            if (edge.first >=nOrVertices || edge.second >= nOrVertices)
                return;
            if (adj.first == -1 || adj.second == -1)
                return;;

            const auto a  = faces(adj.first, 0);
            const auto b  = faces(adj.first, 1);
            const auto c1 = faces(adj.first, 2);
            const auto c2 = faces(adj.second, 2);

            faces.row(adj.first)  << a,  c2, c1;
            faces.row(adj.second) << c2, b,  c1;
        });
    }

    Mesh sqrt31(const Mesh& mesh) {
        const auto& originalVertices{mesh.vertices()};
        const auto& originalFaces{mesh.faces()};

        VertexMatrix vertices(originalVertices.rows() + originalFaces.rows(), 3);
        FaceMatrix faces(3*originalFaces.rows(), 3);

        createMidPoints(vertices, faces, mesh);
        movePoints(vertices, mesh);
        flipEdges(faces, mesh);


        return {vertices, faces};


    }
}
