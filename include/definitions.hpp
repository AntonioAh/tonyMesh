#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP


#include <Eigen/Core>
#include <Eigen/Eigenvalues>

namespace tonyMesh {
    typedef Eigen::Matrix<size_t, Eigen::Dynamic, 3> FaceMatrix;
    typedef Eigen::Vector<size_t, Eigen::Dynamic> FaceVector;
    typedef Eigen::Vector<size_t, Eigen::Dynamic> VertexVector;

    template <typename Scalar>
    using PointMatrix = Eigen::Matrix<Scalar, Eigen::Dynamic, 3>;

    template <typename Scalar>
    using Point = Eigen::RowVector<Scalar, 3>;

    template <typename Scalar>
    using ScalarVector = Eigen::Vector<Scalar, Eigen::Dynamic>;

    template <typename Scalar>
    using ScalarMatrix = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>;

}

#endif
