#include <pca.hpp>

using namespace tonyMesh;

template<typename Scalar>
PCA<Scalar>::PCA(const PointMatrix<Scalar> &points, bool selfAdjacent) : _isSelfAdj(selfAdjacent){
    const auto cov{points.transpose() * points};

    if (_isSelfAdj) {
        _solver = Eigen::SelfAdjointEigenSolver<PointMatrix<Scalar>>(cov);
        if (std::get<Eigen::SelfAdjointEigenSolver<PointMatrix<Scalar>>>(_solver).info() != Eigen::Success) {
            _isValid = false;
            return;
        }
    }
    else {
        _solver = Eigen::EigenSolver<PointMatrix<Scalar>>(cov);
        if (std::get<Eigen::EigenSolver<PointMatrix<Scalar>>>(_solver).info() != Eigen::Success) {
            _isValid = false;
            return;
        }
    }

    _isValid = true;

}

template<typename Scalar>
bool PCA<Scalar>::isValid() const {
    return _isValid;
}
