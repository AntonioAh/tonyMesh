#ifndef TONYMESH_RECONSTRUCT_HPP
#define TONYMESH_RECONSTRUCT_HPP

#include <array>
#include <optional>
#include <expected>

#include <definitions.hpp>
#include <mesh.hpp>

#include "accelerationStructures.hpp"

namespace tonyMesh {
    enum class ReconstructMethod {
        RBF, MLS, Poisson
    };

    enum class ReconstructionError{
        Generic, Normals
    };

    template <typename Scalar>
    struct ReconstructionInfo {
        ReconstructMethod method{ReconstructMethod::MLS};
        std::array<size_t, 3> resolution{{100, 100, 100}};
        std::optional<PointMatrix<Scalar>> normals{};
        std::optional<UniformGrid<Scalar>> grid{};
        size_t neighborsNumber{3};
    };

    template <typename Scalar>
    std::expected<Mesh<Scalar>, ReconstructionError> reconstruct(const PointMatrix<Scalar>& points, ReconstructionInfo<Scalar>& info);

}
#endif //TONYMESH_RECONSTRUCT_HPP
