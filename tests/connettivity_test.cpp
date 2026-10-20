#include <gtest/gtest.h>
#include <definitions.hpp>
#include <mesh.hpp>
#include <Eigen/Dense>
#include <igl/read_triangle_mesh.h>
#include <read_mesh.hpp>

#include <igl/edge_topology.h>
#include <igl/adjacency_list.h>
#include <igl/is_boundary_edge.h>
#include <igl/is_edge_manifold.h>
#include <igl/boundary_facets.h>

using namespace tonyMesh;

class ConnettivityTest : public ::testing::TestWithParam<std::string>{
protected:
    static inline tonyMesh::Mesh mesh;
    static inline Eigen::MatrixXf V;
    static inline Eigen::Matrix<FaceType, Eigen::Dynamic, 3> F;
    static inline std::string fileName;

    void SetUp() override {
        std::string curr = GetParam();

        if (curr != fileName) {
            fileName = curr;
            mesh = readMesh(fileName, MESH_FILE_TYPE::OFF).value();
            igl::read_triangle_mesh(fileName, V, F);
        }
    }
};

TEST_P(ConnettivityTest, dimensions) {
    EXPECT_EQ(mesh.numVertices(), V.rows());
    EXPECT_EQ(mesh.numFaces(), F.rows());
}

TEST_P(ConnettivityTest, vertices) {
    const auto nV{mesh.numVertices()};
    const auto& vertices{mesh.getVertices()};

    #pragma omp parallel for
    for (Index v=0; v < nV; v++) {
        EXPECT_EQ(vertices.row(v), V.row(v));
        EXPECT_EQ(vertices.row(v), mesh.getVertex(v));
    }
}

TEST_P(ConnettivityTest, faces) {
    const auto nF{mesh.numFaces()};
    const auto& faces{mesh.getFaces()};

    # pragma omp parallel for
    for (Index f=0; f < nF; f++) {
        EXPECT_EQ(faces.row(f), F.row(f));
        EXPECT_EQ(faces.row(f), mesh.getFace(f));
    }
}

TEST_P(ConnettivityTest, vertexVertex) {
    const auto nV{mesh.numVertices()};
    std::vector<std::vector<Index>> truth;
    igl::adjacency_list(F, truth);

    const auto valences = mesh.vertexValences();
    EXPECT_EQ(valences.size(), nV);

    const std::vector<std::vector<Index>> my = mesh.vertexVertexAdjacency();

#pragma omp parallel for
    for (Index v=0; v < nV; v++) {
        EXPECT_EQ(my[v], truth[v]);
        EXPECT_EQ(mesh.vertexVertexNeighbors(v), truth[v]);
        EXPECT_EQ(mesh.vertexValence(v), truth[v].size());
        EXPECT_EQ(valences[v], truth[v].size());
    }
}

TEST_P(ConnettivityTest, boundary) {
    if (! igl::is_edge_manifold(F))
        return;


    const auto boundEdges = mesh.boundaryEdges();
    const auto edges = mesh.edges();

    Eigen::MatrixXi EV, FE, EF;
    Eigen::Vector<bool, Eigen::Dynamic> truthBoundary;
    igl::edge_topology(V, F, EV, FE, EF);
    igl::is_boundary_edge(EV, F, truthBoundary);

    EXPECT_EQ(edges.size(), EV.rows());
#pragma omp parallel for
    for (Index e=0; e < EV.rows(); e++) {
        Edge ed{EV(e, 0), EV(e, 1)};
        EXPECT_TRUE(std::ranges::binary_search(edges, ed));
        EXPECT_EQ(mesh.isBoundaryEdge(ed), truthBoundary(e));
    }
}

TEST_P(ConnettivityTest, closedMesh) {
    Eigen::MatrixXi bEdges;
    igl::boundary_facets(F, bEdges);

    EXPECT_EQ(mesh.isClosed(), bEdges.rows() == 0);
}

INSTANTIATE_TEST_SUITE_P(
    MeshDataset,
    ConnettivityTest,
    ::testing::Values(
        "hound.off",
        "luigi.off"
    )
);