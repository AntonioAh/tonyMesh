#ifndef TONYMESH_PCA_HPP
#define TONYMESH_PCA_HPP

#include <definitions.hpp>
#include <variant>
#include <Eigen/Eigenvalues>

namespace tonyMesh {
    template <typename Scalar>
    class PCA {
    public:
        PCA(const PointMatrix<Scalar>& points, bool selfAdjacent=false);
        bool isValid() const;

        ScalarVector<Scalar>& getValues() const;
        Scalar getValue(size_t idx) const;

        ScalarMatrix<Scalar> getVectors() const;
        ScalarVector<Scalar> getVector(size_t idx) const;

    private:
        bool _isSelfAdj, _isValid;
        std::variant<Eigen::EigenSolver<PointMatrix<Scalar>>, Eigen::SelfAdjointEigenSolver<PointMatrix<Scalar>>> _solver;

    };
}

#endif //TONYMESH_PCA_HPP
