#include <gtest/gtest.h>

#include <core/common.hpp>
#include <core/mesh.hpp>
#include <core/dcel.hpp>
#include <core/readMesh.hpp>
#include <igl/read_triangle_mesh.h>
#include <igl/orient_outward.h>

using namespace tonyMesh;

class DcelTest : public testing::TestWithParam<std::string>{
protected:
    Mesh mesh;
    std::shared_ptr<DCEL> dcel;

    void SetUp() override {
        const std::string& fileName = GetParam();
        auto filePathOpt{findDataFile(fileName)};

        if (!filePathOpt.has_value())
            throw std::runtime_error("could not load data file");

        igl::read_triangle_mesh(filePathOpt.value(), mesh.vertices(), mesh.faces());
        mesh.buildDCEL();
        dcel = mesh.dcel();
    }

    void print() {
        const auto dcel{mesh.dcel()};
        const auto halfEdges{dcel->halfEdges()};

        for (size_t i{}; i < dcel->faces().size(); i++) {
            std::cout << i << " : ";
            auto curr{dcel->faces()[i]};

            do {
                std::cout << halfEdges[curr].target << " ";
                curr = halfEdges[curr].next;
            }while (curr != dcel->faces()[i]);
            std::cout << "\n";
        }

        const auto& outFaces{dcel->outerFaces()};

        for (auto a : outFaces ) {
            std::cout << "-1 : ";
            auto curr{a};
            do {
                std::cout << halfEdges[curr].target << " ";
                curr = halfEdges[curr].next;
            } while (curr != a);
            std::cout << "\n";
        }
    }
};

TEST_P(DcelTest, First) {
    for (VertexIndex i{}; i < mesh.numVertices(); i++) {
        auto neigs{mesh.getNeighbors(i)};
        auto dNegih{mesh.dcel()->getNeighbors(i)};

        std::ranges::sort(neigs);
        std::ranges::sort(dNegih);

        EXPECT_EQ(dNegih.size(), neigs.size());
        for (size_t j{}; j < neigs.size(); j++)
            EXPECT_EQ(dNegih[j], neigs[j]);
    }
}


TEST_P(DcelTest, twin) {
    EXPECT_EQ(dcel->faces().size(), mesh.numFaces());
    const auto& halfEdges{dcel->halfEdges()};

    for (size_t i{}; i < halfEdges.size(); i++)
        EXPECT_EQ(halfEdges[halfEdges[i].twin].twin ,i);
}

TEST_P(DcelTest, prevAndNext) {
    EXPECT_EQ(dcel->faces().size(), mesh.numFaces());
    const auto& halfEdges{dcel->halfEdges()};

    for (size_t i{}; i < halfEdges.size(); i++) {
        EXPECT_EQ(halfEdges[halfEdges[i].next].prev, i);
        EXPECT_EQ(halfEdges[halfEdges[i].prev].next, i);
    }
}

TEST_P(DcelTest, getAdjacentFaces) {
    for (VertexIndex i{}; i < mesh.numVertices(); i++) {
        auto f{mesh.getAdjancentFaces(i)};
        auto dcelFaces{dcel->getIncidentFaces(i)};

        std::ranges::sort(f);
        std::ranges::sort(dcelFaces);

        EXPECT_EQ(dcelFaces.size(), f.size());
        for (size_t j{}; j < f.size() ;j++)
            EXPECT_EQ(dcelFaces[j], f[j]);
    }
}

TEST_P(DcelTest, boudaryEdges) {
    auto edges{mesh.boundaryEdges()};
    auto dcelEdges{dcel->boundaryEdges()};

    EXPECT_EQ(dcelEdges.size(), edges.size());
    std::sort(edges.begin(), edges.end());
    std::sort(dcelEdges.begin(), dcelEdges.end());

    for (size_t i{}; i < edges.size(); i++) {
        EXPECT_EQ(dcelEdges[i].first, edges[i].first);
        EXPECT_EQ(dcelEdges[i].second, edges[i].second);
    }
}

TEST_P(DcelTest, boundaryVertices) {
    for (VertexIndex i{}; i < mesh.numVertices(); i++)
        EXPECT_EQ(dcel->isOnBoundary(i), mesh.isOnBoundary(i));
}

TEST_P(DcelTest, vertexVertex) {
    const auto vertexVertex{mesh.vertexVertexAdjacency()};
    const auto dcelVV{dcel->vertexVertexAdjacency(true)};

    ASSERT_EQ(dcelVV.size(), vertexVertex.size());
    for (size_t i{}; i < vertexVertex.size(); i++) {
        ASSERT_EQ(dcelVV[i].size(), vertexVertex[i].size());
        for (size_t j{}; j < vertexVertex[i].size(); j++)
            ASSERT_EQ(dcelVV[i][j], vertexVertex[i][j]);
    }
}


INSTANTIATE_TEST_SUITE_P(
    DCEL,
    DcelTest,
    testing::Values("easy.off", "bunny.off", "honda.off")
);