#include <tbb/parallel_for.h>

#include <Eigen/Geometry>
#include <cmath>
#include <tonyMesh/common.hpp>
#include <tonyMesh/connectivity.hpp>
#include <tonyMesh/normals.hpp>
#include <vector>

namespace tonyMesh::normals {

NormalMatrix faceNormals(const VertexMatrix& vertices, const FaceMatrix& faces) {
    const auto numFaces{faces.rows()};
    NormalMatrix result(numFaces, 3);

    tbb::parallel_for(Index{}, numFaces, [&](Index f) {
        Eigen::Vector3<Real> u(vertices.row(faces(f, 1)) - vertices.row(faces(f, 0)));
        Eigen::Vector3<Real> v(vertices.row(faces(f, 2)) - vertices.row(faces(f, 0)));

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

        // atan2(|a x b|, a . b) stays accurate for very small and near-180 degree angles.
        weights(static_cast<Index>(i)) = std::atan2(a.cross(b).norm(), a.dot(b));
    }
    return weights;
}

// Weight of each face in `incident` = the area of that face.
// weights(i) belongs to incident[i]. `vert` is unused: area does not depend on it.
Eigen::VectorX<Real> buildArea(Index /*vert*/, const std::vector<Index>& incident,
                               const VertexMatrix& vertices,
                               const FaceMatrix& faces) {
    Eigen::VectorX<Real> weights(static_cast<Index>(incident.size()));

    for (size_t i{}; i < incident.size(); i++) {
        const Index f{incident[i]};
        const Eigen::Vector3<Real> u(vertices.row(faces(f, 1)) - vertices.row(faces(f, 0)));
        const Eigen::Vector3<Real> v(vertices.row(faces(f, 2)) - vertices.row(faces(f, 0)));

        weights(static_cast<Index>(i)) = Real{0.5} * u.cross(v).norm();
    }
    return weights;
}
}  // namespace

NormalMatrix vertexNormals(const VertexMatrix& vertices, const FaceMatrix& faces,
                     Normalization type) {
    if (type == Normalization::NO_NORMALIZATION)
        return noNorm(vertices, faces);

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
            sum += coeff(static_cast<Index>(i)) * faceNorms.row(incident[i]).transpose();
        }
        result.row(v) = sum.normalized();
    });

    return result;
}
}  // namespace tonyMesh::normals
