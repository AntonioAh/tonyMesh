module;
#include <array>
#include <optional>
#include <expected>

export module reconstruction;

import definitions;
import mesh;
import accelerationStructures;
import pca;

export namespace tonyMesh {

    enum class ReconstructionError {
        Generic, Normals
    };

    enum class ReconstructionMethod {
        RBF ,MLS, Poisson
    };

    template <typename  Scalar>
    struct ReconstructionInfo {
        ReconstructionMethod method{ReconstructionMethod::MLS};
        std::array<size_t, 3> resolution{{100, 100, 100}};
        std::optional<PointMatrix<Scalar>> normals{std::nullopt};
        size_t normalsNeigh{3};
    };

        template <typename Scalar>
        std::expected<Mesh<Scalar>, ReconstructionError> reconstruct(PointMatrix<Scalar>& _points, ReconstructionInfo<Scalar>& info);

}

namespace tonyMesh {

    template <typename Scalar>
    bool buildNormals(PointMatrix<Scalar>& points, ReconstructionInfo<Scalar>& info) {
        if (info)
            return true;

        UniformGrid grid(points);
        const auto neighs{info.normalsNeigh};

        typename Mesh<Scalar>::PointMatrix normals(points.rows(), 3);

#pragma omp parallel for
        for (auto i{0uz}; i < points.rows(); ++i) {
            auto neighbors{grid.getNeighbors(points.row(i), 3)};
            const PCA pca(neighbors);
            if (!pca.isValid())
                return false;

            normals.row(i) = pca.getVector(0).transpose();
        }

        info.normals = std::move(normals);
        return true;
    }

    template <typename Scalar>
    std::expected<Mesh<Scalar>, ReconstructionError> reconstruct(PointMatrix<Scalar>& _points, ReconstructionInfo<Scalar>& info) {
        if (!buildNormals(_points, info))
            return std::unexpected(ReconstructionError::Normals);


        return std::unexpected(ReconstructionError::Generic);
    }
}

export template class tonyMesh::ReconstructionInfo<float>;
export template class tonyMesh::ReconstructionInfo<double>;
export template class tonyMesh::ReconstructionInfo<long double>;

export template std::expected<tonyMesh::Mesh<float>, tonyMesh::ReconstructionError> tonyMesh::reconstruct(PointMatrix<float>&, ReconstructionInfo<float>&);
export template std::expected<tonyMesh::Mesh<double>, tonyMesh::ReconstructionError> tonyMesh::reconstruct(PointMatrix<double>&, ReconstructionInfo<double>&);
export template std::expected<tonyMesh::Mesh<long double>, tonyMesh::ReconstructionError> tonyMesh::reconstruct(PointMatrix<long double>&, ReconstructionInfo<long double>&);
