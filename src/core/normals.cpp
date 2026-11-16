#include <tbb/parallel_for.h>

#include <Eigen/Eigenvalues>
#include <Eigen/Geometry>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <tonyMesh/accelerationStructures/kdTree.hpp>
#include <tonyMesh/common.hpp>
#include <tonyMesh/connectivity.hpp>
#include <tonyMesh/normals.hpp>
#include <unordered_set>
#include <vector>

#include "tonyMesh/graph.hpp"

namespace tonyMesh::normals {

NormalMatrix faceNormals(const VertexMatrix& vertices,
                         const FaceMatrix& faces) {
    const auto numFaces{faces.rows()};
    NormalMatrix result(numFaces, 3);

    tbb::parallel_for(Index{}, numFaces, [&](Index f) {
        Eigen::Vector3<Real> u(vertices.row(faces(f, 1)) -
                               vertices.row(faces(f, 0)));
        Eigen::Vector3<Real> v(vertices.row(faces(f, 2)) -
                               vertices.row(faces(f, 0)));

        result.row(f) = u.cross(v).normalized();
    });

    return result;
}

namespace {
// Sum of the unit normals of the faces around each vertex, no weighting.
NormalMatrix noNorm(const VertexMatrix& vertices, const FaceMatrix& faces) {
    const auto faceNorms = faceNormals(vertices, faces);
    const auto numVertices{vertices.rows()};
    NormalMatrix result(numVertices, 3);

    const auto vf{connectivity::vertex_faces_lists(numVertices, faces)};

    tbb::parallel_for(Index{}, numVertices, [&](Index v) {
        Eigen::Vector3<Real> sum{0, 0, 0};

        for (Index f : vf[static_cast<size_t>(v)]) {
            sum += faceNorms.row(f);
        }
        result.row(v) = sum.normalized();
    });

    return result;
}

// Weight of each face in `incident` = the angle of that face at vertex `vert`.
// weights(i) belongs to incident[i].
Eigen::VectorX<Real> buildAngles(Index vert, const std::vector<Index>& incident,
                                 const VertexMatrix& vertices,
                                 const FaceMatrix& faces) {
    Eigen::VectorX<Real> weights(static_cast<Index>(incident.size()));

    for (size_t i{}; i < incident.size(); i++) {
        const Index f{incident[i]};

        // Corner of face f that is `vert`.
        int k{};
        while (k < 2 && faces(f, k) != vert) k++;

        const Eigen::Vector3<Real> a(vertices.row(faces(f, (k + 1) % 3)) -
                                     vertices.row(vert));
        const Eigen::Vector3<Real> b(vertices.row(faces(f, (k + 2) % 3)) -
                                     vertices.row(vert));

        // atan2(|a x b|, a . b) stays accurate for very small and near-180
        // degree angles.
        weights(static_cast<Index>(i)) =
            std::atan2(a.cross(b).norm(), a.dot(b));
    }
    return weights;
}

// Weight of each face in `incident` = the area of that face.
// weights(i) belongs to incident[i]. `vert` is unused: area does not depend on
// it.
Eigen::VectorX<Real> buildArea(Index /*vert*/,
                               const std::vector<Index>& incident,
                               const VertexMatrix& vertices,
                               const FaceMatrix& faces) {
    Eigen::VectorX<Real> weights(static_cast<Index>(incident.size()));

    for (size_t i{}; i < incident.size(); i++) {
        const Index f{incident[i]};
        const Eigen::Vector3<Real> u(vertices.row(faces(f, 1)) -
                                     vertices.row(faces(f, 0)));
        const Eigen::Vector3<Real> v(vertices.row(faces(f, 2)) -
                                     vertices.row(faces(f, 0)));

        weights(static_cast<Index>(i)) = Real{0.5} * u.cross(v).norm();
    }
    return weights;
}
}  // namespace

NormalMatrix vertexNormals(const VertexMatrix& vertices,
                           const FaceMatrix& faces, Normalization type) {
    if (type == Normalization::NO_NORMALIZATION) return noNorm(vertices, faces);

    const auto faceNorms = faceNormals(vertices, faces);
    const auto numVertices{vertices.rows()};
    NormalMatrix result(numVertices, 3);

    const auto vf{connectivity::vertex_faces_lists(numVertices, faces)};
    const auto build = type == Normalization::ANGLE ? buildAngles : buildArea;

    tbb::parallel_for(Index{}, numVertices, [&](Index v) {
        const auto& incident{vf[static_cast<size_t>(v)]};
        const auto coeff = build(v, incident, vertices, faces);

        Eigen::Vector3<Real> sum{0, 0, 0};
        for (size_t i{}; i < incident.size(); i++) {
            sum += coeff(static_cast<Index>(i)) *
                   faceNorms.row(incident[i]).transpose();
        }
        result.row(v) = sum.normalized();
    });

    return result;
}
NormalMatrix pointCloud(const VertexMatrix& vertices, size_t numNeighbors,
                        std::shared_ptr<AccelerationStructure> acc) {
    if (numNeighbors < 3)
        throw std::runtime_error(
            "cannot compute with fewer than 3 neighbors. Needed for the "
            "covariance matrix.");

    if (!acc) acc = std::make_shared<accelerationStructures::KdTree>(vertices);
    const auto numVertices{vertices.rows()};
    NormalMatrix result(numVertices, 3);
    const size_t kWithSelf{numNeighbors + 1};

    tbb::parallel_for(Index{}, numVertices, [&](Index vert) {
        const std::vector<Index> neighbors{
            acc->knn(vertices.row(vert), kWithSelf)};

        Eigen::Matrix3<Real> outer{Eigen::Matrix3<Real>::Zero()};
        Eigen::Vector3<Real> sum{Eigen::Vector3<Real>::Zero()};

        const Eigen::Vector3<Real> q{vertices.row(vert)};
        for (Index neigh : neighbors) {
            const Eigen::Vector3<Real> d = vertices.row(neigh).transpose() - q;
            sum += d;
            outer.noalias() += d * d.transpose();
        }
        const Real invK{static_cast<Real>(1) / neighbors.size()};
        const Eigen::Vector3<Real> mean = sum * invK;
        const Eigen::Matrix3<Real> cov = outer * invK - mean * mean.transpose();

        Eigen::SelfAdjointEigenSolver<Eigen::Matrix3<Real>> solver;
        solver.computeDirect(cov);

        result.row(vert) = solver.eigenvectors().col(0).transpose();
    });

    orientNormals(result, vertices, numNeighbors, acc);

    return result;
}
void orientNormals(NormalMatrix& normals, const VertexMatrix& vertices,
                   size_t numNeighbors,
                   std::shared_ptr<AccelerationStructure> acc) {
    if (!acc) acc = std::make_shared<accelerationStructures::KdTree>(vertices);

    const size_t numVertices(vertices.rows());
    const Index numVertI{vertices.rows()};

    std::vector<std::tuple<size_t, size_t, Real>> allEdges;
    for (Index vert{}; vert < numVertI; vert++) {
        const auto neighbors{acc->knn(vertices.row(vert), numNeighbors + 1)};

        for (Index neigh : neighbors) {
            if (neigh == vert) continue;
            const Index m{std::min(vert, neigh)}, M{std::max(vert, neigh)};
            allEdges.emplace_back(
                m, M, 1 - std::abs(normals.row(m).dot(normals.row(M))));
        }
    }
    std::ranges::sort(allEdges);
    const auto uniques{std::unique(
        allEdges.begin(), allEdges.end(), [](const auto& a, const auto& b) {
            return std::get<0>(a) == std::get<0>(b) &&
                   std::get<1>(a) == std::get<1>(b);
        })};
    allEdges.erase(uniques, allEdges.end());

    std::vector<std::pair<size_t, size_t>> mst{
        graphs::mst(numVertices, allEdges)};
    std::vector<std::vector<size_t>> mstAdjListd{
        graphs::buildAdjList(numVertices, mst)};
    std::vector connectedComponents{
        graphs::connectedComponents(numVertices, mst)};

    constexpr size_t none{std::numeric_limits<size_t>::max()};

    std::vector<size_t> top(numVertices, none);
    for (size_t i{}; i < numVertices; i++) {
        const auto comp{connectedComponents[i]};

        if (top[comp] == none || vertices(i, 2) > vertices(top[comp], 2)) {
            top[comp] = i;
        }
    }

    std::unordered_set<size_t> compsDone;
    std::vector<size_t> order;
    // Shared by all components: they are disjoint, so no reset is needed.
    std::vector<size_t> parent(numVertices, none);
    for (size_t vert{}; vert < numVertices; vert++) {
        const auto component{connectedComponents[vert]};
        if (compsDone.contains(component)) continue;
        compsDone.insert(component);

        Index startingPoint{static_cast<Index>(top[component])};
        graphs::dfsOrder(order, parent, numVertices, startingPoint,
                         mstAdjListd);

        if (normals(startingPoint, 2) < 0) normals.row(startingPoint) *= -1;

        for (size_t i(1); i < order.size(); i++) {
            if (normals.row(parent[order[i]]).dot(normals.row(order[i])) < 0)
                normals.row(order[i]) *= -1;
        }
    }
}
}  // namespace tonyMesh::normals
