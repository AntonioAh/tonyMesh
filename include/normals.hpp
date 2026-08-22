#ifndef TONYMESH_NORMALS_HPP
#define TONYMESH_NORMALS_HPP

#include <definitions.hpp>
#include <accelerationStructures.hpp>
#include <expected>

namespace tonyMesh {
    enum class NormalsError {
        Generic, Orientation
    };

    template <typename Scalar>
    std::expected<PointMatrix<Scalar>, NormalsError> computeNormals(const PointMatrix<Scalar>& points, std::optional<UniformGrid<Scalar>>& grid, size_t neighNumber = 3u, bool orient=false, size_t k=5uz);

    template <typename Scalar>
    std::expected<PointMatrix<Scalar>, NormalsError> computeNormals(const PointMatrix<Scalar>& points, UniformGrid<Scalar>& grid);
}

#endif //TONYMESH_NORMALS_HPP
