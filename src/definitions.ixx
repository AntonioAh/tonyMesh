module;
#include <Eigen/Dense>

export module definitions;

export namespace tonyMesh {
    typedef Eigen::Matrix<size_t, Eigen::Dynamic, 3> FaceMatrix;

    template <typename Scalar>
    using PointMatrix = Eigen::Matrix<Scalar, Eigen::Dynamic, 3>;

    template <typename Scalar>
    using Point = Eigen::Vector<Scalar, 3>;

}