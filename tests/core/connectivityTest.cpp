#include <gtest/gtest.h>
#include <igl/read_triangle_mesh.h>
#include <igl/adjacency_list.h>
#include <igl/vertex_triangle_adjacency.h>
#include <igl/edges.h>
#include <igl/is_boundary_edge.h>

#include <Eigen/Core>
#include <core/common.hpp>
#include <core/mesh.hpp>
#include <core/readMesh.hpp>

#include <vector>


using namespace tonyMesh;

class ConnectivityTest : public testing::Test {
protected:
    VertexMatrix vertices;
    FaceMatrix faces;
    Mesh mesh;

    ConnectivityTest(){
        auto filePathOpt{findDataFile("honda.off")};

        if (!filePathOpt.has_value())
            throw std::runtime_error("could not load data file");

        std::cout << "loaded\n";
        igl::read_triangle_mesh(filePathOpt.value(), vertices, faces);
        mesh = tonyMesh::Mesh(vertices, faces);
    }
};

TEST_F(ConnectivityTest, copyConstructor) {
    std::cout << mesh.numVertices() << " " << mesh.numFaces() << "\n";
    EXPECT_EQ(mesh.numVertices(), vertices.rows());
    EXPECT_EQ(mesh.vertices().rows(), vertices.rows());

    EXPECT_EQ(mesh.numFaces(), faces.rows());
    EXPECT_EQ(mesh.faces().rows(), faces.rows());

    for (Index i{}; i < vertices.rows(); i++) {
        EXPECT_EQ(mesh.vertices().row(i).size(), 3);
        for (Index j{}; j < 3; j++)
            EXPECT_EQ(mesh.vertices()(i, j), vertices(i, j));
    }

    for (Index i{}; i < faces.rows(); i++) {
        EXPECT_EQ(mesh.faces().row(i).size(), 3);
        for (Index j{} ;j < 3; j++)
            EXPECT_EQ(mesh.faces()(i, j), faces(i, j));
    }
}

TEST_F(ConnectivityTest, vertexNeighbors) {
    std::vector<std::vector<int>> adjacency;
    igl::adjacency_list(faces, adjacency);

    auto myAdj{mesh.vertexVertexAdjacency()};

    EXPECT_EQ(myAdj.size(), adjacency.size());
    for (VertexIndex i{}; i < vertices.rows(); i++) {
        auto curr{mesh.getNeighbors(i)};

        EXPECT_EQ(curr.size(), adjacency[i].size());
        EXPECT_EQ(myAdj[i].size(), adjacency[i].size());

        for (size_t j{}; j < myAdj[i].size(); j++) {
            EXPECT_EQ(myAdj[i][j], adjacency[i][j]);
            EXPECT_EQ(curr[j], adjacency[i][j]);
        }
    }
}

TEST_F(ConnectivityTest, vertexFace) {
    std::vector<std::vector<int>> adjacency, other;
    igl::vertex_triangle_adjacency(vertices, faces, adjacency, other);

    auto myAdj{mesh.vertexFaceAdjacency()};
    EXPECT_EQ(myAdj.size(), adjacency.size());
    for (VertexIndex i{}; i < vertices.rows(); i++) {
        auto curr{mesh.getAdjancentFaces(i)};

        EXPECT_EQ(curr.size(), adjacency[i].size());
        EXPECT_EQ(myAdj[i].size(), adjacency[i].size());

        for (size_t j{}; j < myAdj[i].size(); j++) {
            EXPECT_EQ(myAdj[i][j], adjacency[i][j]);
            EXPECT_EQ(curr[j], adjacency[i][j]);
        }
    }
}

TEST_F(ConnectivityTest, edges) {
    Eigen::MatrixXi edges;
    igl::edges(faces, edges);

    auto myEdges{mesh.edges()};
    EXPECT_EQ(myEdges.size(), edges.rows());

    std::vector<Edge> buildEdges;
    buildEdges.reserve(myEdges.size());
    for (Index i{}; i < edges.rows(); i++)
        buildEdges.emplace_back(edges(i, 0), edges(i, 1));

    std::ranges::sort(buildEdges, [](const auto& a, const auto& b) {
        return a < b;
    });

    for (size_t i{}; i < myEdges.size(); i++) {
        EXPECT_EQ(myEdges[i].first, buildEdges[i].first);
        EXPECT_EQ(myEdges[i].second, buildEdges[i].second);
    }
    std::cout << "total edges : " << myEdges.size();
}

TEST_F(ConnectivityTest, boundaryEdges) {
    Eigen::MatrixXi edges;
    Eigen::VectorXi isBoundary;

    igl::edges(faces, edges);
    igl::is_boundary_edge(edges, faces, isBoundary);

    auto myBoundary{mesh.boundaryEdges()};

    size_t counter{};
    for (Index i{}; i < isBoundary.size(); i++) {
        if (isBoundary(i)) {
            counter++;

            bool found{std::ranges::find(myBoundary, Edge{edges(i, 0), edges(i, 1)}) != myBoundary.end()};
            EXPECT_TRUE(found);

            EXPECT_TRUE(mesh.isOnBoundary(myBoundary[i].first));
            EXPECT_TRUE(mesh.isOnBoundary(myBoundary[i].second));
        }
    }
    std::cout << "boundary edges : " << myBoundary.size() << "\n";
    EXPECT_EQ(myBoundary.size(), counter);
}

TEST_F(ConnectivityTest, isBoundaryEdges_first) {
    Eigen::MatrixXi edges;
    Eigen::VectorXi isBoundary;
    igl::edges(faces, edges);
    igl::is_boundary_edge(edges, faces, isBoundary);

    auto myEdges{mesh.edges()};
    auto myIsBound{mesh.isBoundaryEdge(myEdges)};

    EXPECT_EQ(myEdges.size(), edges.rows());
    size_t counter{};

    for (Index i{}; i < edges.rows(); i++) {
        counter++;

        Edge e{edges(i, 0), edges(i, 1)};
        auto idx{std::lower_bound(myEdges.begin(), myEdges.end(), e) - myEdges.begin()};

        EXPECT_EQ(myIsBound[idx], isBoundary(i));
    }
}

TEST_F(ConnectivityTest, isBoundaryEdges_second) {
    Eigen::MatrixXi edges;
    Eigen::VectorXi isBoundary;
    igl::edges(faces, edges);
    igl::is_boundary_edge(edges, faces, isBoundary);

    auto myEdges{mesh.edges()};
    auto myIsBound{mesh.isBoundaryEdge(myEdges)};

    auto [otherEdge, otherBoun] = mesh.isBoundaryEdge();
    EXPECT_EQ(otherEdge.size(), myEdges.size());
    EXPECT_EQ(otherBoun.size(), myIsBound.size());
    EXPECT_EQ(otherEdge.size(), otherBoun.size());

    for (size_t i{}; i < otherEdge.size(); i++) {
        EXPECT_EQ(otherEdge[i], myEdges[i]);
        EXPECT_EQ(otherBoun[i], myIsBound[i]);
    }
}